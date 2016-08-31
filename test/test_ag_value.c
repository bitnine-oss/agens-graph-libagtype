#include <stdio.h>
#include "agtype.h"
#include "json.h" /* FIXME */
#include "CuTest.h"

struct pair {
	char *key;
	char *val;
};

void Test_foreach(CuTest *tc)
{
	struct pair expects[3] = {
		{"sitename", "\"joys of programming\""}, 
		{"tags", "[ \"c\", \"c++\", \"java\", \"PHP\" ]"},
		{"author-details", "{ \"name\": \"Joys of Programming\", \"Number of Posts\": 10 }"}
	};
	int i;
	const char *key;
	ag_value value;
	char *string = "{\"sitename\" : \"joys of programming\", \"tags\" : [ \"c\" , \"c++\", \"java\", \"PHP\" ], \"author-details\": { \"name\" : \"Joys of Programming\", \"Number of Posts\" : 10 } }";
	ag_value props = json_tokener_parse(string);

	i = 0;
	ag_value_object_foreach(props, key, value) {
		CuAssertStrEquals(tc, expects[i].key, key);
		CuAssertStrEquals(tc, expects[i].val, ag_value_to_string((ag_value)value));
		++i;
	}
}
