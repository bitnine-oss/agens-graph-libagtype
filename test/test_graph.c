#include <stdio.h>
#include <string.h>
#include "agtype.h"
#include "CuTest.h"

void Test_ag_vertex(CuTest *tc)
{
	char *data = "person[100.10]{\"name\":\"ktlee\",\"age\":41}";
	struct ag_vertex *vx = ag_vertex_new(data);
	CuAssertStrEquals(tc, "person", vx->label);
	CuAssertIntEquals(tc, 100, vx->vid.oid);
	CuAssertIntEquals(tc, 10, vx->vid.id);
	CuAssertStrEquals(tc, "{ \"name\": \"ktlee\", \"age\": 41 }", ag_json_to_string(vx->props));
	CuAssertTrue(tc, !ag_json_get_int(ag_json_object_get(vx->props, "name")));
	CuAssertIntEquals(tc, 41, ag_json_get_int(ag_json_object_get(vx->props, "age")));
	ag_vertex_free(vx);
}

void Test_ag_path(CuTest *tc)
{
	char *data = "company[100.10]{\"name\":\"bitnine\"},"
		"employee[200.20][100.10,300.30]{},"
		"person[300.30]{\"name\":\"ktlee\",\"age\":41}";
	struct ag_path *path;
	struct ag_vertex *vx;
	
	path = ag_path_new(data);
	CuAssertIntEquals(tc, 1, ag_path_get_size(path));
	vx = ag_path_get_start(path);
	CuAssertStrEquals(tc, "company", vx->label);
	CuAssertIntEquals(tc, 100, vx->vid.oid);
	CuAssertIntEquals(tc, 10, vx->vid.id);
	CuAssertStrEquals(tc, "{ \"name\": \"bitnine\" }", ag_json_to_string(vx->props));
	vx = ag_path_get_end(path);
	CuAssertStrEquals(tc, "person", vx->label);
	CuAssertIntEquals(tc, 300, vx->vid.oid);
	CuAssertIntEquals(tc, 30, vx->vid.id);
	CuAssertStrEquals(tc, "{ \"name\": \"ktlee\", \"age\": 41 }", ag_json_to_string(vx->props));
	CuAssertStrEquals(tc, "company[100.10]{ \"name\": \"bitnine\" }employee[200.20][100.10,300.30]{ }person[300.30]{ \"name\": \"ktlee\", \"age\": 41 }", ag_path_to_string(path));
	ag_path_free(path);
}

void Test_ag_json_null(CuTest *tc)
{
	char *data = "person[100.10]{\"name\":\"ktlee\",\"age\":null}";
	struct ag_vertex *vx = ag_vertex_new(data);
	CuAssertStrEquals(tc, "person", vx->label);
	CuAssertIntEquals(tc, 100, vx->vid.oid);
	CuAssertIntEquals(tc, 10, vx->vid.id);
	CuAssertStrEquals(tc, "{ \"name\": \"ktlee\", \"age\": null }", ag_json_to_string(vx->props));
	CuAssertTrue(tc, !ag_json_get_int(ag_json_object_get(vx->props, "name")));
	CuAssertIntEquals(tc, 0, ag_json_get_int(ag_json_object_get(vx->props, "age")));
	CuAssertTrue(tc, ag_json_is_null(ag_json_object_get(vx->props, "age")));
	ag_vertex_free(vx);
}

