#include <stdio.h>
#include "agtype.h"
#include "CuTest.h"

struct pair {
	char *key;
	char *val;
};

void Test_null(CuTest *tc)
{
	ag_json null1 = ag_json_from_string("null");
	ag_json null2 = ag_json_new_null();
	CuAssertTrue(tc, NULL != null1);
	CuAssertTrue(tc, NULL != null2);
	ag_json_deref(null2);
	CuAssertTrue(tc, ag_json_is_null(null2));
}

void Test_boolean(CuTest *tc)
{
	ag_json my_bool = ag_json_from_string("true");
	CuAssertTrue(tc, NULL != my_bool);
	CuAssertIntEquals(tc, AG_JSON_TRUE, ag_json_get_boolean(my_bool));
}

void Test_foreach(CuTest *tc)
{
	struct pair expects[] = {
		{"sitename", "\"joys of programming\""}, 
		{"tags", "[ \"c\", \"c++\", \"java\", \"PHP\" ]"},
		{"author-details", "{ \"name\": \"Joys of Programming\", \"Number of Posts\": 10 }"},
		{"NullVal", "null"}
	};
	int i;
	const char *key;
	ag_json value;
	char *string = "{\"sitename\" : \"joys of programming\", \"tags\" : [ \"c\" , \"c++\", \"java\", \"PHP\" ], \"author-details\": { \"name\" : \"Joys of Programming\", \"Number of Posts\" : 10 }, \"NullVal\": null } }";
	ag_json props = ag_json_from_string(string);

	i = 0;
	ag_json_object_foreach(props, key, value) {
		CuAssertStrEquals(tc, expects[i].key, key);
		CuAssertStrEquals(tc, expects[i].val, ag_json_to_string((ag_json)value));
		++i;
	}
}

void Test_ref(CuTest *tc)
{
	char *string = "{\"sitename\" : \"joys of programming\", \"tags\" : [ \"c\" , \"c++\", \"java\", \"PHP\" ], \"author-details\": { \"name\" : \"Joys of Programming\", \"Number of Posts\" : 10 }, \"NullVal\": null } }";
	ag_json props = ag_json_from_string(string);
	ag_json sitename = ag_json_object_get(props, "sitename");
	ag_json_ref(sitename);
	ag_json_deref(props);
	CuAssertTrue(tc, ag_json_is_string(sitename));
	CuAssertStrEquals(tc, "joys of programming", ag_json_get_string(sitename));
}

void Test_object_new(CuTest *tc)
{
	ag_json person = ag_json_new_object();
	ag_json_object_add(person, "name", ag_json_new_string("ktlee"));
	ag_json_object_add(person, "age", ag_json_new_int(41));
	CuAssertStrEquals(tc, "{ \"name\": \"ktlee\", \"age\": 41 }", ag_json_to_string(person));
}

void Test_array_new(CuTest *tc)
{
	ag_json arr = ag_json_new_array();
	ag_json_array_append(arr, ag_json_new_true());
	ag_json_array_append(arr, ag_json_new_double(0.5));
	ag_json_array_append(arr, NULL);
	CuAssertStrEquals(tc, "[ true, 0.5, null ]", ag_json_to_string(arr));
}
