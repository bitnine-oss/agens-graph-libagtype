

/* This is auto-generated code. Edit at your own peril. */
#include <stdio.h>
#include <stdlib.h>

#include "CuTest.h"


extern void Test_foreach(CuTest*);
extern void Test_ag_vertex(CuTest*);
extern void Test_ag_path(CuTest*);
extern void Test_get_token(CuTest*);
extern void Test_get_path_token(CuTest*);


void RunAllTests(void) 
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();


    SUITE_ADD_TEST(suite, Test_foreach);
    SUITE_ADD_TEST(suite, Test_ag_vertex);
    SUITE_ADD_TEST(suite, Test_ag_path);
    SUITE_ADD_TEST(suite, Test_get_token);
    SUITE_ADD_TEST(suite, Test_get_path_token);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    CuStringDelete(output);
    CuSuiteDelete(suite);
}

int main(void)
{
    RunAllTests();
}

