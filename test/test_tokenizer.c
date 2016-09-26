#include <stdio.h>
#include <string.h>
#include "tokenizer.h"
#include "CuTest.h"

void Test_get_token(CuTest *tc)
{
	char *data = "person[100.10]{\"name\"=\"ktlee\",\"age\"=41}";
	char *results[] = {
		"person", "[100.10]", "{\"name\"=\"ktlee\",\"age\"=41}"
	};
	token t;
	int i;
	char buffer[100];

	token_init(&t, data, strlen(data));

	i = 0;
	while (get_token(&t)) 
	{
		copy_token_str(buffer, &t);
		CuAssertStrEquals(tc, results[i], buffer);
		++i;
	}
}

void Test_get_path_token(CuTest *tc)
{
	char *data = "company[100.10]{\"name\"=\"bitnine\"},"
		"employee[200.20][100.10,300.30]{},"
		"person[300.30]{\"name\"=\"ktlee\",\"age\"=41}";
	char *results[] = {
		"company[100.10]{\"name\"=\"bitnine\"}",
		"employee[200.20][100.10,300.30]{}",
		"person[300.30]{\"name\"=\"ktlee\",\"age\"=41}"
	};
	token t;
	int i;
	char buffer[100];

	token_init(&t, data, strlen(data));

	i = 0;
	while (get_path_token(&t))
	{
		copy_token_str(buffer, &t);
		CuAssertStrEquals(tc, results[i], buffer);
		++i;
	}

}
