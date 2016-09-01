#include "agtype.h"
#include "json.h"

ag_value_type
ag_value_get_type(ag_value val)
{
	return json_object_get_type(val);
}

ag_value_bool 
ag_value_get_boolean(ag_value val)
{
	return json_object_get_boolean(val);
}

double 
ag_value_get_double(ag_value val)
{
	if (! ag_value_is_double(val))
		return 0;

	return json_object_get_double(val);
}

int 
ag_value_get_int(ag_value val)
{
	if (! ag_value_is_int(val))
		return 0;

	return json_object_get_int(val);
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

ag_value
ag_value_array_get(ag_value arr, int idx)
{
	if (! ag_value_is_array(arr))
		return NULL;

	return json_object_array_get_idx(arr, idx);
}

const char *
ag_value_get_string(ag_value val)
{
	if (! ag_value_is_string(val))
		return NULL;

	return json_object_get_string(val);
}

int
ag_value_object_size(ag_value obj)
{
	if (! ag_value_is_object(obj))
		return 0;

	return json_object_object_length(obj);
}

int 
ag_value_array_size(ag_value arr)
{
	if (! ag_value_is_array(arr))
		return 0;

	return json_object_array_length(arr);
}

/* iteraotr */

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
