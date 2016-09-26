#include <stdio.h>
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
set_gid(struct ag_gid *gid, const char *data)
{
	const char *p = data + 1; /* skip "[" or "," */
	char *e;
	gid->oid = strtol(p, &e, 10);
	gid->id = strtol(e + 1, &e, 10);
}

static ag_json
make_ag_json(token *tok)
{
	return ag_json_from_string_ex(tok->start, token_length(tok));
}

struct ag_vertex *
ag_vertex_new(const char *data)
{
	return ag_vertex_new_ex(data, strlen(data));
}

struct ag_vertex *
ag_vertex_new_ex(const char *data, int len)
{
	struct ag_vertex *v;
	token t;

	v = (struct ag_vertex *)malloc(sizeof(struct ag_vertex));
	token_init(&t, data, len);
	get_token(&t);
	v->label = strndup(t.start, token_length(&t));
	get_token(&t);
	set_gid(&v->vid, t.start);
	get_token(&t);
	v->props = make_ag_json(&t);
	v->str = NULL;

	return v;
}

const char *
ag_vertex_to_string(struct ag_vertex *v)
{
	int len = 1; /* null */
	char buffer[256];
	const char *prop;
	char *str;

	if (v == NULL)
		return NULL;

	if (v->str != NULL)
		free((void *)v->str);

	len += sprintf(buffer, "%s[%d.%d]", v->label, v->vid.oid, v->vid.id);	
	prop = ag_json_to_string(v->props);	
	len += strlen(prop);

	str = (char *)malloc(len);
	strcpy(str, buffer);
	strcat(str, prop);
	v->str = str;

	return v->str;
}

void
ag_vertex_free_(void *data)
{
	if (data) 
	{
		struct ag_vertex *v = (struct ag_vertex *)data;
		free(v->label);
		if (v->str)
			free((void *)v->str);
		ag_json_deref(v->props);
		free(v);
	}
}

void
ag_vertex_free(struct ag_vertex *vertex)
{
	ag_vertex_free_((void *)vertex);
}

struct ag_edge *
ag_edge_new(const char *data)
{
	return ag_edge_new_ex(data, strlen(data));
}

struct ag_edge *
ag_edge_new_ex(const char *data, int len)
{
	struct ag_edge *e;
	token t;

	e = (struct ag_edge *)malloc(sizeof(struct ag_edge));
	token_init(&t, data, len);
	get_token(&t);
	e->label = strndup(t.start, token_length(&t));
	get_token(&t);
	set_gid(&e->eid, t.start);
	get_token(&t);
	set_gid(&e->startVid, t.start);
	set_gid(&e->endVid, strchr(t.start, ','));
	get_token(&t);
	e->props = make_ag_json(&t);
	e->str = NULL;

	return e;
}

const char *
ag_edge_to_string(struct ag_edge *e)
{
	int len = 1; /* null */
	char buffer[1024];
	const char *prop;
	char *str;

	if (e == NULL)
		return NULL;

	if (e->str != NULL)
		free((void *)e->str);

	len += sprintf(buffer, "%s[%d.%d][%d.%d,%d.%d]", e->label, 
			e->eid.oid, e->eid.id, e->startVid.oid, e->startVid.id,
			e->endVid.oid, e->endVid.id);	
	prop = ag_json_to_string(e->props);
	len += strlen(prop);

	str = malloc(len);
	strcpy(str, buffer);
	strcat(str, prop);
	e->str = str;

	return e->str;
}

void
ag_edge_free_(void *data)
{
	if (data) 
	{
		struct ag_edge *e = (struct ag_edge *)data;
		free(e->label);
		if (e->str)
			free((void *)e->str);
		ag_json_deref(e->props);
		free(e);
	}
}

void
ag_edge_free(struct ag_edge *edge)
{
	ag_edge_free_((void *)edge);
}

struct ag_path *
ag_path_new(const char *data)
{
	return ag_path_new_ex(data, strlen(data));
}

struct ag_path *
ag_path_new_ex(const char *data, int len)
{
	struct ag_path *p;
	token t;
	int i;
	char *buf;
	int buf_len;

	p = (struct ag_path *)malloc(sizeof(struct ag_path));
	p->str = NULL;
	p->vertices = array_list_new(ag_vertex_free_);
	p->edges = array_list_new(ag_edge_free_);
	token_init(&t, data, len);
	i = 0;
	buf_len = 1024;
	buf = malloc(buf_len + 1);
	while (get_path_token(&t))
	{
		if (token_length(&t) > buf_len)
		{
			buf_len = token_length(&t);
			buf = realloc(buf, buf_len + 1);
		}
		copy_token_str(buf, &t);
		if (i % 2 == 0)
			array_list_add(p->vertices, ag_vertex_new(buf));
		else
			array_list_add(p->edges, ag_edge_new(buf));
		++i;
	}
	free(buf);
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

static void
no_free(void *data)
{
	return; /* empty */
}

const char *
ag_path_to_string(struct ag_path *p)
{
	int i;
	int len = 1; /* null */
	struct array_list *strs;
	char *str;
	char *c;

	if (p == NULL)
		return NULL;

	if (p->str != NULL)
		free((void *)p->str);

	strs = array_list_new(no_free);

	for (i = 0; i < array_list_length(p->edges); ++i) {
		str = (char *)ag_vertex_to_string(array_list_get_idx(p->vertices, i));
		len += strlen(str);
		array_list_add(strs, str);
		str = (char *)ag_edge_to_string(array_list_get_idx(p->edges, i));
		len += strlen(str);
		array_list_add(strs, str);
	}

	str = (char *)ag_vertex_to_string(array_list_get_idx(p->vertices, i));
	len += strlen(str);
	array_list_add(strs, str);

	c = str = malloc(len);
	for (i = 0; i < array_list_length(strs); ++i) {
		c += sprintf(c, "%s", (char *)array_list_get_idx(strs, i));
	}
	*c = '\0';
	p->str = str;
	array_list_free(strs);

	return p->str;
}

void
ag_path_free(struct ag_path *path)
{
	if (path) 
	{
		array_list_free(path->vertices);
		array_list_free(path->edges);
		if (path->str)
			free((void *)path->str);
		free(path);
	}
}

