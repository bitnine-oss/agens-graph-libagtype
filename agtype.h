#ifndef _AGTYPE_H
#define _AGTYPE_H

#include <inttypes.h>

/* graph types */

typedef void * ag_json; 

struct ag_gid {
	int oid;
	int id;
};

struct ag_vertex {
	char *label;
	struct ag_gid vid;
	ag_json props;
	const char *str;
};

struct ag_vertex *ag_vertex_new(char *data);
const char *ag_vertex_to_string(struct ag_vertex *v);
void ag_vertex_free(struct ag_vertex *v);

struct ag_edge {
	char *label;
	struct ag_gid eid;
	struct ag_gid startVid;
	struct ag_gid endVid;
	ag_json props;
	const char *str;
};

struct ag_edge *ag_edge_new(char *data);
const char *ag_edge_to_string(struct ag_edge *e);
void ag_edge_free(struct ag_edge *e);

struct ag_path {
	struct array_list *vertices;
	struct array_list *edges;
	const char *str;
};

struct ag_path* ag_path_new(char *data);
int ag_path_get_size(struct ag_path *path);
struct ag_vertex *ag_path_get_start(struct ag_path *path);
struct ag_vertex *ag_path_get_end(struct ag_path *path);
const char *ag_path_to_string(struct ag_path *p);
void ag_path_free(struct ag_path *p);

/* value types */

typedef enum {
	AG_JSON_FALSE,
	AG_JSON_TRUE
} ag_json_bool;

typedef enum {
  AG_JSON_NULL,
  AG_JSON_BOOLEAN,
  AG_JSON_DOUBLE,
  AG_JSON_INT,
  AG_JSON_OBJECT,
  AG_JSON_ARRAY,
  AG_JSON_STRING
} ag_json_type;

ag_json_type ag_json_get_type(ag_json val);
#define ag_json_is_null(v)    (ag_json_get_type(v) == AG_JSON_NULL)
#define ag_json_is_boolean(v) (ag_json_get_type(v) == AG_JSON_BOOLEAN)
#define ag_json_is_double(v)  (ag_json_get_type(v) == AG_JSON_DOUBLE)
#define ag_json_is_int(v)     (ag_json_get_type(v) == AG_JSON_INT)
#define ag_json_is_object(v)  (ag_json_get_type(v) == AG_JSON_OBJECT)
#define ag_json_is_array(v)   (ag_json_get_type(v) == AG_JSON_ARRAY)
#define ag_json_is_string(v)  (ag_json_get_type(v) == AG_JSON_STRING)

/* constructor */

ag_json ag_json_from_string(const char *jstr);

/* null */
ag_json ag_json_new_null(void);

/* boolean */

ag_json ag_json_new_true(void);
ag_json ag_json_new_false(void);
ag_json_bool ag_json_get_boolean(ag_json val);

/* double */

ag_json ag_json_new_double(double val);
double ag_json_get_double(ag_json val);

/* int */

ag_json ag_json_new_int(int64_t val);
int64_t ag_json_get_int(ag_json val);

/* object */

ag_json ag_json_new_object(void);
void ag_json_object_add(ag_json obj, const char *key, ag_json val);
ag_json ag_json_object_get(ag_json obj, const char *key);
void ag_json_object_del(ag_json obj, const char *key);
int ag_json_object_size(ag_json obj);

/* array */

ag_json ag_json_new_array(void);
/* Returns: 0 if OK, −1 on error. */
int ag_json_array_append(ag_json arr, ag_json elem);
/* int ag_json_array_put(ag_json arr, int idx, ag_json elem); */
ag_json ag_json_array_get(ag_json arr, int idx);
int ag_json_array_size(ag_json arr);

/* string */
ag_json ag_json_new_string(const char *str);
const char *ag_json_get_string(ag_json val);

/* iterator */

typedef void * ag_json_iter;

ag_json_iter ag_json_object_iter_begin(ag_json val);
ag_json_iter ag_json_object_iter_next(ag_json_iter iter);
const char *ag_json_object_iter_key(ag_json_iter iter);
const ag_json ag_json_object_iter_value(ag_json_iter iter);

#define ag_json_object_foreach(obj, key, val) \
    for(ag_json_iter cur_ = ag_json_object_iter_begin(obj); \
		(NULL != cur_) \
			&& (key = ag_json_object_iter_key(cur_)) \
			&& (val = ag_json_object_iter_value(cur_)); \
		cur_ = ag_json_object_iter_next(cur_))

/* reference counting functions */

ag_json ag_json_ref(ag_json obj);

/* returns 1 if the val was freed */
int ag_json_deref(ag_json obj);

/* utils */

const char *ag_json_to_string(ag_json val);

#endif /* _AGTYPE_H */
