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

	return json_object_array_idx(arr, idx);
}

char *
ag_value_get_string(ag_value val)
{
	if (! ag_value_is_string(string))
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

/* utils */

char *
ag_value_to_string(ag_value val)
{
	return json_object_to_json_string(val);
}
