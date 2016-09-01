#ifndef _AGTYPE_H
#define _AGTYPE_H

#include <inttypes.h>

/* graph types */

typedef void * ag_value; 

struct ag_gid {
	int oid;
	int id;
};

struct ag_vertex {
	char *label;
	struct ag_gid vid;
	ag_value props;
};

struct ag_vertex *ag_vertex_new(char *data);

struct ag_edge {
	char *label;
	struct ag_gid eid;
	struct ag_gid startVid;
	struct ag_gid endVid;
	ag_value props;
};

struct ag_edge *ag_edge_new(char *data);

struct ag_path {
	struct array_list *vertices;
	struct array_list *edges;
};

struct ag_path* ag_path_new(char *data);
int ag_path_get_size(struct ag_path *path);
struct ag_vertex *ag_path_get_start(struct ag_path *path);
struct ag_vertex *ag_path_get_end(struct ag_path *path);

/* value types */

typedef enum {
	AG_VALUE_FALSE,
	AG_VALUE_TRUE
} ag_value_bool;

typedef enum {
  AG_VALUE_NULL,
  AG_VALUE_BOOLEAN,
  AG_VALUE_DOUBLE,
  AG_VALUE_INT,
  AG_VALUE_OBJECT,
  AG_VALUE_ARRAY,
  AG_VALUE_STRING
} ag_value_type;

ag_value_type ag_value_get_type(ag_value val);
#define ag_value_is_null(v)    (ag_value_get_type(v) == AG_VALUE_NULL)
#define ag_value_is_boolean(v) (ag_value_get_type(v) == AG_VALUE_BOOLEAN)
#define ag_value_is_double(v)  (ag_value_get_type(v) == AG_VALUE_DOUBLE)
#define ag_value_is_int(v)     (ag_value_get_type(v) == AG_VALUE_INT)
#define ag_value_is_object(v)  (ag_value_get_type(v) == AG_VALUE_OBJECT)
#define ag_value_is_array(v)   (ag_value_get_type(v) == AG_VALUE_ARRAY)
#define ag_value_is_string(v)  (ag_value_get_type(v) == AG_VALUE_STRING)

/* constructor */

ag_value ag_value_from_json_string(const char *jstr);

/* boolean */

ag_value ag_value_true_new(void);
ag_value ag_value_false_new(void);
ag_value_bool ag_value_get_boolean(ag_value val);

/* double */

ag_value ag_value_double_new(double val);
double ag_value_get_double(ag_value val);

/* int */

ag_value ag_value_int_new(int64_t val);
int64_t ag_value_get_int(ag_value val);

/* object */

ag_value ag_value_object_new(void);
void ag_value_object_put(ag_value obj, const char *key, ag_value elem);
ag_value ag_value_object_get(ag_value obj, const char *key);
void ag_value_object_del(ag_value obj, const char *key);
int ag_value_object_size(ag_value obj);

/* array */

ag_value ag_value_array_new(void);
/* Returns: 0 if OK, −1 on error. */
int ag_value_array_append(ag_value arr, ag_value elem);
/* int ag_value_array_put(ag_value arr, int idx, ag_value elem); */
ag_value ag_value_array_get(ag_value arr, int idx);
int ag_value_array_size(ag_value arr);

/* string */
ag_value ag_value_string_new(const char *str);
const char *ag_value_get_string(ag_value val);

/* iterator */

typedef void * ag_value_iter;

ag_value_iter ag_value_object_iter_begin(ag_value val);
ag_value_iter ag_value_object_iter_next(ag_value_iter iter);
const char *ag_value_object_iter_key(ag_value_iter iter);
const ag_value ag_value_object_iter_value(ag_value_iter iter);

#define ag_value_object_foreach(obj, key, val) \
    for(ag_value_iter cur_ = ag_value_object_iter_begin(obj); \
		(NULL != cur_) \
			&& (key = ag_value_object_iter_key(cur_)) \
			&& ((val = ag_value_object_iter_value(cur_)) \
				|| val == NULL); \
		cur_ = ag_value_object_iter_next(cur_))


/* reference counting functions */

ag_value ag_value_ref(ag_value obj);

/* returns 1 if the val was freed */
int ag_value_deref(ag_value obj);

/* utils */

const char *ag_value_to_string(ag_value val);

#endif /* _AGTYPE_H */
