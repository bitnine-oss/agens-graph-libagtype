#include "agtype.h"
#include "json.h"

/* constructor */

ag_json
ag_json_from_string(const char *jstr)
{
	return json_tokener_parse(jstr);
}

ag_json
ag_json_from_string_ex(const char *jstr, int len)
{
	struct json_tokener *tok;
	ag_json json;
	
	tok = json_tokener_new();
	json = json_tokener_parse_ex(tok, jstr, len);
	switch (json_tokener_get_error(tok))
	{
	case json_tokener_success:
		break;
	case json_tokener_continue:
		json = json_tokener_parse_ex(tok, " ", -1);
		if (json_tokener_get_error(tok) != json_tokener_success)
			json = NULL;
		break;
	default:
		json = NULL;
	}
	json_tokener_free(tok);

	return json;
}

ag_json_type
ag_json_get_type(ag_json val)
{
	return json_object_get_type(val);
}

/* null */
ag_json
ag_json_new_null(void)
{
	return json_object_new_null();
}

/* boolean */

ag_json
ag_json_new_true(void)
{
	return json_object_new_true();
}

ag_json
ag_json_new_false(void)
{
	return json_object_new_false();
}

ag_json_bool 
ag_json_get_boolean(ag_json val)
{
	return json_object_get_boolean(val);
}

/* double */

ag_json
ag_json_new_double(double val)
{
	return json_object_new_double(val);
}

double 
ag_json_get_double(ag_json val)
{
	if (! ag_json_is_double(val))
		return 0;

	return json_object_get_double(val);
}

/* number */

ag_json
ag_json_new_int(int64_t val)
{
	return json_object_new_int(val);
}

int64_t
ag_json_get_int(ag_json val)
{
	if (! ag_json_is_int(val))
		return 0;

	return json_object_get_int(val);
}

/* object */

ag_json
ag_json_new_object(void)
{
	return json_object_new_object();
}

void 
ag_json_object_add(ag_json obj, const char *key, ag_json val)
{
	json_object_object_add(obj, key, val);
}

ag_json
ag_json_object_get(ag_json obj, const char *key)
{
	json_object *value;

	if (! ag_json_is_object(obj))
		return NULL;

	if (json_object_object_get_ex(obj, key, &value))
		return (ag_json)value;
	else
		return NULL;
}

void
ag_json_object_del(ag_json obj, const char *key)
{
	json_object_object_del(obj, key);
}

int
ag_json_object_size(ag_json obj)
{
	if (! ag_json_is_object(obj))
		return 0;

	return json_object_object_length(obj);
}

/* array */

ag_json
ag_json_new_array(void)
{
	return json_object_new_array();
}

int
ag_json_array_append(ag_json arr, ag_json elem)
{
	return json_object_array_add(arr, elem);
}

/*
int
ag_json_array_put(ag_json arr, int idx, ag_json elem)
{
	return json_object_array_put_idx(arr, idx, elem);
}
*/

ag_json
ag_json_array_get(ag_json arr, int idx)
{
	if (! ag_json_is_array(arr))
		return NULL;

	return json_object_array_get_idx(arr, idx);
}

int 
ag_json_array_size(ag_json arr)
{
	if (! ag_json_is_array(arr))
		return 0;

	return json_object_array_length(arr);
}

/* string */

ag_json
ag_json_new_string(const char *str)
{
	return json_object_new_string(str);
}

const char *
ag_json_get_string(ag_json val)
{
	if (! ag_json_is_string(val))
		return NULL;

	return json_object_get_string(val);
}

/* iterator */

ag_json_iter 
ag_json_object_iter_begin(ag_json val)
{
	struct lh_table *t;
	if (val == NULL)
		return NULL;

	if (! ag_json_is_object(val))
		return NULL;

	t = json_object_get_object(val);
	if (t == NULL)
		return NULL;
	
	return t->head;
}

ag_json_iter 
ag_json_object_iter_next(ag_json_iter iter)
{
	struct lh_entry *e;

	if (iter == NULL)
		return NULL;
	
	e = (struct lh_entry *)iter;

	return e->next;
}

const char *
ag_json_object_iter_key(ag_json_iter iter)
{
	if (iter == NULL)
		return NULL;

	return (const char *)((struct lh_entry *)iter)->k;
}

const ag_json 
ag_json_object_iter_value(ag_json_iter iter)
{
	if (iter == NULL)
		return NULL;

	return (const ag_json)((struct lh_entry *)iter)->v;
}

/* reference counting functions */

ag_json 
ag_json_ref(ag_json val)
{
	return json_object_get(val);
}

/* returns 1 if the val was freed */
int 
ag_json_deref(ag_json obj)
{
	return json_object_put(obj);
}

/* utils */

const char *
ag_json_to_string(ag_json val)
{
	return json_object_to_json_string(val);
}
