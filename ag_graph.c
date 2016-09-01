#include <string.h>
#include <stdlib.h>
#include "arraylist.h"
#include "tokenizer.h"
#include "json.h"
#include "agtype.h"

/*
 * [oid.id], [oid.id, or ,oid.id]
 */
static void
set_gid(struct ag_gid *gid, char *data)
{
	char *p = data + 1; /* skip "[" or "," */
	char *e;
	gid->oid = strtol(p, &e, 10);
	gid->id = strtol(e + 1, &e, 10);
}

static ag_value
make_ag_value(token *tok)
{
	struct json_tokener *jtok;
	json_object *p;

	jtok = json_tokener_new();
	p = json_tokener_parse_ex(jtok, tok->start, token_length(tok));
	if (json_tokener_get_error(jtok) != json_tokener_success) 
		p = NULL;
	json_tokener_free(jtok);

	return (ag_value)p;
}

struct ag_vertex *
ag_vertex_new(char *data)
{
	struct ag_vertex *v;
	token t;

	v = (struct ag_vertex *)malloc(sizeof(struct ag_vertex));
	token_init(&t, data);
	get_token(&t);
	v->label = strndup(t.start, token_length(&t));
	get_token(&t);
	set_gid(&v->vid, t.start);
	get_token(&t);
	v->props = make_ag_value(&t);

	return v;
}

void
ag_vertex_free_(void *data)
{
	if (data) 
	{
		struct ag_vertex *v = (struct ag_vertex *)data;
		free(v->label);
		free(v->props);
		free(v);
	}
}

void
ag_vertex_free(struct ag_vertex *vertex)
{
	ag_vertex_free_((void *)vertex);
}

struct ag_edge *
ag_edge_new(char *data)
{
	struct ag_edge *e;
	token t;

	e = (struct ag_edge *)malloc(sizeof(struct ag_edge));
	token_init(&t, data);
	get_token(&t);
	e->label = strndup(t.start, token_length(&t));
	get_token(&t);
	set_gid(&e->eid, t.start);
	get_token(&t);
	set_gid(&e->startVid, t.start);
	set_gid(&e->endVid, strchr(t.start, ','));
	get_token(&t);
	e->props = make_ag_value(&t);

	return e;
}

void
ag_edge_free_(void *data)
{
	if (data) 
	{
		struct ag_edge *e = (struct ag_edge *)data;
		free(e->label);
		free(e->props);
		free(e);
	}
}

void
ag_edge_free(struct ag_edge *edge)
{
	ag_edge_free_((void *)edge);
}

struct ag_path *
ag_path_new(char *data)
{
	struct ag_path *p;
	token t;
	int i;
	char *buffer;
	int len;

	p = (struct ag_path *)malloc(sizeof(struct ag_path));
	p->vertices = array_list_new(ag_vertex_free_);
	p->edges = array_list_new(ag_edge_free_);
	token_init(&t, data);
	i = 0;
	len = 1024;
	buffer = malloc(len + 1);
	while (get_path_token(&t))
	{
		if (token_length(&t) > len)
		{
			len = token_length(&t);
			buffer = realloc(buffer, len + 1);
		}
		copy_token_str(buffer, &t);
		if (i % 2 == 0)
			array_list_add(p->vertices, ag_vertex_new(buffer));
		else
			array_list_add(p->edges, ag_edge_new(buffer));
		++i;
	}
	free(buffer);
	return p;
}

int
ag_path_get_size(struct ag_path *path)
{
	return array_list_length(path->edges);
}

struct ag_vertex *
ag_path_get_start(struct ag_path *path)
{
	return (struct ag_vertex *)array_list_get_idx(path->vertices, 0);
}

struct ag_vertex *
ag_path_get_end(struct ag_path *path)
{
	int i = array_list_length(path->vertices);
	if (i == 0)
		return NULL;
	else
		return (struct ag_vertex *)array_list_get_idx(path->vertices, i - 1);
}

void
ag_path_free(struct ag_path *path)
{
	if (path) 
	{
		array_list_free(path->vertices);
		array_list_free(path->edges);
		free(path);
	}
}
