#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <stdlib.h>
#include "agtype.h"
#include "agodbc.h"
#include "CuTest.h"

#define STR_LEN 128 + 1
#define REM_LEN 254 + 1

/* global vars */

SQLHENV env;
SQLHDBC dbc;
SQLHSTMT hstmt;
SQLRETURN rc;
SQLCHAR outstr[1024];
SQLSMALLINT outstrlen;

#define check_error(ms) \
	do { \
		if (! SQL_SUCCEEDED(rc)) \
		{ \
			extract_error(ms, hstmt, SQL_HANDLE_STMT); \
			CuFail(tc, ms); \
		} \
	} while (0)

static void extract_error(char *fn, SQLHANDLE handle, SQLSMALLINT type)
{
    SQLINTEGER i = 0;
    SQLINTEGER native;
    SQLCHAR	state[7];
    SQLCHAR	text[256];
    SQLSMALLINT	len;
    SQLRETURN ret;

    fprintf(stderr,
            "\n"
            "The driver reported the following diagnostics whilst running "
            "%s\n\n",
            fn);

	rc = SQLGetDiagRec(type, handle, ++i, state, &native, text,
			sizeof(text), &len );
	if (SQL_SUCCEEDED(rc))
		printf("%s:%d:%d:%s\n", state, i, native, text);
}

static SQLRETURN connectDB(void)
{
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
	SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
	rc = SQLDriverConnect(dbc, NULL, "DSN=AgensGraph;", SQL_NTS,
			outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_COMPLETE);
	if (! SQL_SUCCEEDED(rc)) 
	{
		extract_error("SQLDriverConnect", dbc, SQL_HANDLE_STMT);
		return SQL_ERROR;
	}
	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
	if (! SQL_SUCCEEDED(rc)) 
	{
		extract_error("Stmt Handle Alloc", dbc, SQL_HANDLE_STMT);
		return SQL_ERROR;
	}
	return SQL_SUCCESS;
}

static void disconnectDB(void)
{
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLFreeHandle(SQL_HANDLE_DBC, dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, env);
}

void Test_MATCH(CuTest *tc)
{
	SQLCHAR *buffer;
	SQLSMALLINT agType;
	SQLLEN agRawDataSize;
	SQLLEN ind;
	struct ag_vertex *v;

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"CREATE (n:person '{\"name\": \"Emil\", \"from\": \"Sweden\", \"klout\": 99}')", SQL_NTS);
	check_error("CREATE");

	SQLFreeStmt(hstmt, SQL_CLOSE);
	
	rc = SQLExecDirect(hstmt, (SQLCHAR*)"MATCH (n:person '{\"from\": \"Sweden\"}') RETURN n", SQL_NTS);
	check_error("MATCH");

	rc = AG_SQLDescribeCol(hstmt, 1, &agType, &agRawDataSize);
	check_error("AG_SQLDescribeCol");

	rc = SQLFetch(hstmt);
	check_error("Fetch");

	buffer = (SQLCHAR *)malloc(agRawDataSize);
	rc = AG_SQLGetData(hstmt, 1, agType, (void **)&v, buffer, agRawDataSize, &ind);
	check_error("AG_SQLGetData");

	CuAssertStrEquals(tc, "Sweden", ag_json_get_string(ag_json_object_get(v->props, "from")));

	free(buffer);
	ag_vertex_free(v);

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}

void Test_RETURN(CuTest *tc)
{
	SQLINTEGER two;
	SQLLEN ind;

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	SQLBindCol(hstmt, 1, SQL_C_SLONG, &two, 0, &ind);
	rc = SQLExecDirect(hstmt, (SQLCHAR*)"RETURN 1+1", SQL_NTS);
	check_error("RETURN");

	SQLFetch(hstmt);
	check_error("SQLFetch");

	CuAssertIntEquals(tc, 2, two);

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}

void Test_Cast_Jsonb_to_Text(CuTest *tc)
{
	SQLLEN ind = SQL_NTS;
	SQLCHAR from[10];

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	SQLBindCol(hstmt, 1, SQL_C_CHAR, from, 10, &ind);
	rc = SQLExecDirect(hstmt, (SQLCHAR*)"MATCH (n:person) RETURN (n).from", SQL_NTS);
	check_error("SQLPrepare");

	rc = SQLFetch(hstmt);
	check_error("SQLFetch");

	CuAssertStrEquals(tc, "\"Sweden\"", from);

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}

void Test_Bind_WHERE(CuTest *tc)
{
	SQLINTEGER two;
	SQLLEN ind = SQL_NTS;
	SQLCHAR from[10] = "Sweden";
	SQLCHAR *buffer;
	SQLSMALLINT agType;
	SQLLEN agRawDataSize;
	struct ag_vertex *v;

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	rc = SQLPrepare(hstmt, (SQLCHAR*)"MATCH (n:person) WHERE (n).from = to_jsonb(?::text) RETURN n", SQL_NTS);
	check_error("SQLPrepare");

	rc = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 10, 0, from, 0, &ind);
	check_error("SQLBindParameter");

	rc = SQLExecute(hstmt);
	check_error("SQLExecute");

	rc = AG_SQLDescribeCol(hstmt, 1, &agType, &agRawDataSize);
	check_error("AG_SQLDescribeCol");

	rc = SQLFetch(hstmt);
	check_error("SQLFetch");

	buffer = (SQLCHAR *)malloc(agRawDataSize);
	rc = AG_SQLGetData(hstmt, 1, agType, (void **)&v, buffer, agRawDataSize, &ind);
	check_error("AG_SQLGetData");

	CuAssertIntEquals(tc, 99, ag_json_get_int(ag_json_object_get(v->props, "klout")));

	free(buffer);
	ag_vertex_free(v);

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}

