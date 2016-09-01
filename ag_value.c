#include "agtype.h"
#include "json.h"

/* constructor */

ag_value
ag_value_from_json_string(const char *jstr)
{
	return json_tokener_parse(jstr);
}

ag_value_type
ag_value_get_type(ag_value val)
{
	return json_object_get_type(val);
}

/* boolean */

ag_value
ag_value_true_new(void)
{
	return json_object_new_boolean(AG_VALUE_TRUE);
}

ag_value
ag_value_false_new(void)
{
	return json_object_new_boolean(AG_VALUE_FALSE);
}

ag_value_bool 
ag_value_get_boolean(ag_value val)
{
	return json_object_get_boolean(val);
}

/* double */

ag_value
ag_value_double_new(double val)
{
	return json_object_new_double(val);
}

double 
ag_value_get_double(ag_value val)
{
	if (! ag_value_is_double(val))
		return 0;

	return json_object_get_double(val);
}

/* number */

ag_value
ag_value_int_new(int val)
{
	return json_object_new_int(val);
}

int 
ag_value_get_int(ag_value val)
{
	if (! ag_value_is_int(val))
		return 0;

	return json_object_get_int(val);
}

/* object */

ag_value
ag_value_object_new(void)
{
	return json_object_new_object();
}

void 
ag_value_object_put(ag_value obj, const char *key, ag_value elem)
{
	json_object_object_add(obj, key, elem);
}

ag_value
ag_value_object_get(ag_value obj, const char *key)
{
	json_object *value;

	if (! ag_value_is_object(obj))
		return NULL;

	if (json_object_object_get_ex(obj, key, &value))
		return (ag_value)value;
	else
		return NULL;
}

void
ag_value_object_del(ag_value obj, const char *key)
{
	json_object_object_del(obj, key);
}

int
ag_value_object_size(ag_value obj)
{
	if (! ag_value_is_object(obj))
		return 0;

	return json_object_object_length(obj);
}

/* array */

ag_value
ag_value_array_new(void)
{
	return json_object_new_array();
}

int
ag_value_array_append(ag_value arr, ag_value elem)
{
	return json_object_array_add(arr, elem);
}

/*
int
ag_value_array_put(ag_value arr, int idx, ag_value elem)
{
	return json_object_array_put_idx(arr, idx, elem);
}
*/

ag_value
ag_value_array_get(ag_value arr, int idx)
{
	if (! ag_value_is_array(arr))
		return NULL;

	return json_object_array_get_idx(arr, idx);
}

int 
ag_value_array_size(ag_value arr)
{
	if (! ag_value_is_array(arr))
		return 0;

	return json_object_array_length(arr);
}

/* string */

ag_value
ag_value_string_new(const char *str)
{
	return json_object_new_string(str);
}

const char *
ag_value_get_string(ag_value val)
{
	if (! ag_value_is_string(val))
		return NULL;

	return json_object_get_string(val);
}

/* iterator */

ag_value_iter 
ag_value_object_iter_begin(ag_value val)
{
	struct lh_table *t;
	if (val == NULL)
		return NULL;

	if (! ag_value_is_object(val))
		return NULL;

	t = json_object_get_object(val);
	if (t == NULL)
		return NULL;
	
	return t->head;
}

ag_value_iter 
ag_value_object_iter_next(ag_value_iter iter)
{
	struct lh_entry *e;

	if (iter == NULL)
		return NULL;
	
	e = (struct lh_entry *)iter;

	return e->next;
}

const char *
ag_value_object_iter_key(ag_value_iter iter)
{
	if (iter == NULL)
		return NULL;

	return (const char *)((struct lh_entry *)iter)->k;
}

const ag_value 
ag_value_object_iter_value(ag_value_iter iter)
{
	if (iter == NULL)
		return NULL;

	return (const ag_value)((struct lh_entry *)iter)->v;
}

/* reference counting functions */

ag_value 
ag_value_ref(ag_value val)
{
	return json_object_get(val);
}

/* returns 1 if the val was freed */
int 
ag_value_deref(ag_value obj)
{
	return json_object_put(obj);
}

/* utils */

const char *
ag_value_to_string(ag_value val)
{
	return json_object_to_json_string(val);
}
