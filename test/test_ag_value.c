#include <stdio.h>
#include "agtype.h"
#include "json.h" /* FIXME */
#include "CuTest.h"

struct pair {
	char *key;
	char *val;
};

void Test_null(CuTest *tc)
{
	ag_value null = json_tokener_parse("null");
	CuAssertTrue(tc, !null);
}

void Test_boolean(CuTest *tc)
{
	ag_value my_bool = json_tokener_parse("true");
	CuAssertTrue(tc, NULL != my_bool);
	CuAssertIntEquals(tc, AG_VALUE_TRUE, ag_value_get_boolean(my_bool));
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
	ag_value value;
	char *string = "{\"sitename\" : \"joys of programming\", \"tags\" : [ \"c\" , \"c++\", \"java\", \"PHP\" ], \"author-details\": { \"name\" : \"Joys of Programming\", \"Number of Posts\" : 10 }, \"NullVal\": null } }";
	ag_value props = json_tokener_parse(string);

	i = 0;
	ag_value_object_foreach(props, key, value) {
		CuAssertStrEquals(tc, expects[i].key, key);
		CuAssertStrEquals(tc, expects[i].val, ag_value_to_string((ag_value)value));
		++i;
	}
}
