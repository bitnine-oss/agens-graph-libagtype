#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <stdlib.h>
#include "agtype.h"
#include "agodbc-type.h"
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
	SQLSMALLINT agType;
	SQLLEN ind;
	struct ag_vertex *v;

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"DROP GRAPH u CASCADE", SQL_NTS);
	rc = SQLExecDirect(hstmt, (SQLCHAR*)"CREATE GRAPH u", SQL_NTS);
	check_error("CREATE GRAPH");
	rc = SQLExecDirect(hstmt, (SQLCHAR*)"SET graph_path = u", SQL_NTS);
	check_error("SET graph_path");
	rc = SQLExecDirect(hstmt, (SQLCHAR*)"DROP VLABEL PERSON", SQL_NTS);

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"CREATE VLABEL PERSON", SQL_NTS);
	check_error("CREATE VLABEL");

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"CREATE (n:person {'name': 'Emil', 'from': 'Sweden', 'klout': 99})", SQL_NTS);
	check_error("CREATE");

	SQLFreeStmt(hstmt, SQL_CLOSE);
	
	rc = SQLExecDirect(hstmt, (SQLCHAR*)"MATCH (n:person {'from': 'Sweden'}) RETURN n", SQL_NTS);
	check_error("MATCH");

	rc = AG_SQLBindCol(hstmt, 1, AG_VERTEX, (SQLPOINTER *)&v, &ind);
	check_error("AG_SQLBindCol");

	rc = SQLFetch(hstmt);
	check_error("Fetch");

	CuAssertStrEquals(tc, "Sweden", ag_json_get_string(ag_json_object_get(v->props, "from")));

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

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"SET graph_path = u", SQL_NTS);
	check_error("SET graph_path");

	SQLBindCol(hstmt, 1, SQL_C_CHAR, from, 10, &ind);
	rc = SQLExecDirect(hstmt, (SQLCHAR*)"MATCH (n:person) RETURN n.from", SQL_NTS);
	check_error("SQLPrepare");

	rc = SQLFetch(hstmt);
	check_error("SQLFetch");

	CuAssertStrEquals(tc, "Sweden", from);

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}

void Test_Bind_WHERE(CuTest *tc)
{
	SQLINTEGER two;
	SQLLEN ind = SQL_NTS;
	SQLCHAR from[10] = "Sweden";
	struct ag_vertex *v;

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"SET graph_path = u", SQL_NTS);
	check_error("SET graph_path");

	rc = SQLPrepare(hstmt, (SQLCHAR*)"MATCH (n:person) WHERE n.from = ? RETURN n", SQL_NTS);
	check_error("SQLPrepare");

	rc = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 10, 0, from, 0, &ind);
	check_error("SQLBindParameter");

	rc = SQLExecute(hstmt);
	check_error("SQLExecute");

	rc = AG_SQLBindCol(hstmt, 1, AG_VERTEX, (SQLPOINTER *)&v, &ind);
	check_error("AG_SQLBindCol");

	rc = SQLFetch(hstmt);
	check_error("SQLFetch");

	CuAssertIntEquals(tc, 99, ag_json_get_int(ag_json_object_get(v->props, "klout")));

	ag_vertex_free(v);

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}

void Test_Bind_WHERE_int(CuTest *tc)
{
	SQLINTEGER two;
	SQLINTEGER klout;
	SQLLEN ind = SQL_NTS;
	SQLLEN klout_ind = 0;
	struct ag_vertex *v;

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"SET graph_path = u", SQL_NTS);
	check_error("SET graph_path");

	rc = SQLPrepare(hstmt, (SQLCHAR*)"MATCH (n:person) WHERE n.klout::int = ? RETURN n", SQL_NTS);
	check_error("SQLPrepare");

	klout = 99;
	rc = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 0, 0, &klout, 0, &klout_ind);
	check_error("AG_SQLBindParameter");

	rc = SQLExecute(hstmt);
	check_error("SQLExecute");

	rc = AG_SQLBindCol(hstmt, 1, AG_VERTEX, (SQLPOINTER *)&v, &ind);
	check_error("AG_SQLBindCol");

	rc = SQLFetch(hstmt);
	check_error("SQLFetch");

	CuAssertIntEquals(tc, 99, ag_json_get_int(ag_json_object_get(v->props, "klout")));

	ag_vertex_free(v);

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}

void Test_Bind_MATCH_Property(CuTest *tc)
{
	SQLINTEGER two;
	SQLLEN ind = SQL_NTS;
	SQLCHAR name[10] = "Emil";
	struct ag_vertex *v;

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"SET graph_path = u", SQL_NTS);
	check_error("SET graph_path");

	rc = SQLPrepare(hstmt, (SQLCHAR*)"MATCH ( n:person { 'name' : ?::text } )  RETURN n", SQL_NTS);
	check_error("SQLPrepare");

	rc = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 10, 0, name, 0, &ind);
	check_error("SQLBindParameter");

	rc = SQLExecute(hstmt);
	check_error("SQLExecute");

	rc = AG_SQLBindCol(hstmt, 1, AG_VERTEX, (SQLPOINTER *)&v, &ind);
	check_error("AG_SQLBindCol");

	rc = SQLFetch(hstmt);
	check_error("SQLFetch");

	CuAssertIntEquals(tc, 99, ag_json_get_int(ag_json_object_get(v->props, "klout")));

	ag_vertex_free(v);

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}

void Test_Bind_MATCH_Property_JSONB_Scalar(CuTest *tc)
{
	SQLINTEGER two;
	SQLLEN ind = SQL_NTS;
	ag_json name;
	struct ag_vertex *v;

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"SET graph_path = u", SQL_NTS);
	check_error("SET graph_path");

	rc = SQLPrepare(hstmt, (SQLCHAR*)"MATCH ( n:person { 'name' : ? } )  RETURN n", SQL_NTS);
	check_error("SQLPrepare");

	name = ag_json_new_string("Emil");
	rc = AG_SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, AG_PROPERTY, name, &ind);
	check_error("AG_SQLBindParameter");

	rc = SQLExecute(hstmt);
	check_error("SQLExecute");

	ag_json_deref(name);

	rc = AG_SQLBindCol(hstmt, 1, AG_VERTEX, (SQLPOINTER *)&v, &ind);
	check_error("AG_SQLBindCol");

	rc = SQLFetch(hstmt);
	check_error("SQLFetch");

	CuAssertIntEquals(tc, 99, ag_json_get_int(ag_json_object_get(v->props, "klout")));

	ag_vertex_free(v);

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}

void Test_Bind_MATCH_Property_JSONB(CuTest *tc)
{
	SQLINTEGER two;
	SQLLEN ind = SQL_NTS;
	SQLCHAR from[10] = "Emil";
	SQLCHAR *buffer;
	SQLSMALLINT agType;
	SQLLEN agRawDataSize;
	struct ag_vertex *v;
	ag_json jobj;

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"SET graph_path = u", SQL_NTS);
	check_error("SET graph_path");

	rc = SQLPrepare(hstmt, (SQLCHAR*)"MATCH ( n:person ? ) RETURN n", SQL_NTS);
	check_error("SQLPrepare");

	jobj = ag_json_new_object();
	ag_json_object_add(jobj, "name", ag_json_new_string("Emil"));
	rc = AG_SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, AG_PROPERTY, jobj, &ind);
	check_error("AG_SQLBindParameter");

	rc = SQLExecute(hstmt);
	check_error("SQLExecute");

	ag_json_deref(jobj);

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

void Test_Bind_ODBC_escape(CuTest *tc)
{
	SQLLEN ind = SQL_NTS;
	ag_json age;

	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"SET graph_path = u", SQL_NTS);
	check_error("SET graph_path");

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"CREATE (n:person {'name': 'Emil', 'from': 'KOREA', 'age': 41})", SQL_NTS);
	check_error("CREATE");

	SQLFreeStmt(hstmt, SQL_CLOSE);
	
	rc = SQLExecDirect(hstmt, 
		(SQLCHAR*)"MATCH ( n:person { 'from' : {fn UCASE('Korea')} } ) "
		"RETURN n.age", SQL_NTS);
	check_error("SQLExecDirect");

	rc = AG_SQLBindCol(hstmt, 1, AG_PROPERTY, (SQLPOINTER *)&age, &ind);
	check_error("AG_SQLBindCol");

	rc = SQLFetch(hstmt);
	check_error("SQLFetch");

	CuAssertIntEquals(tc, 41, ag_json_get_int(age));

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}

void Test_DELETE(CuTest *tc)
{
	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"SET graph_path = u", SQL_NTS);
	check_error("SET graph_path");

	rc = SQLExecDirect(hstmt, (SQLCHAR*)"MATCH (n:person {'from': 'KOREA'}) DELETE n", SQL_NTS);
	check_error("DELETE");

	SQLFreeStmt(hstmt, SQL_CLOSE);

	disconnectDB();
}


/* from "psqlodbc-09.05.0400/test/src/" */

#define bindParamString(hstmt, paramno, str) \
	_bindParamString(hstmt, paramno, str, tc)

#define bindOutParamString(hstmt, paramno, outbuf, outbuflen) \
	_bindOutParamString(hstmt, paramno, outbuf, outbuflen, tc)

#define executeQuery(hstmt) \
	_executeQuery(hstmt, tc)

#define prepareQuery(hstmt, str) \
	_prepareQuery(hstmt, str, tc)

static void
_bindParamString(HSTMT hstmt, int paramno, char *str, CuTest *tc)
{
	SQLRETURN	rc;
	static SQLLEN		cbParams[10];

	cbParams[paramno] = SQL_NTS;
	rc = SQLBindParameter(hstmt, paramno, SQL_PARAM_INPUT,
						  SQL_C_CHAR,	/* value type */
						  SQL_CHAR,		/* param type */
						  20,			/* column size */
						  0,			/* dec digits */
						  str,		/* param value ptr */
						  0,			/* buffer len */
						  &cbParams[paramno]		/* StrLen_or_IndPtr */);
	check_error("SQLBindParameter failed");
}

static void
_bindOutParamString(HSTMT hstmt, int paramno, char *outbuf, int outbuflen, CuTest *tc)
{
	static SQLLEN cbParams[10];

    rc = SQLBindParameter(hstmt, paramno, SQL_PARAM_OUTPUT,
						  SQL_C_CHAR,	/* value type */
						  SQL_CHAR,		/* param type */
						  20,			/* column size */
						  0,			/* dec digits */
						  outbuf,		/* param value ptr */
						  outbuflen,	/* buffer len */
						  &cbParams[paramno]		/* StrLen_or_IndPtr */);
	check_error("SQLBindParameter failed");
}

static void
_executeQuery(HSTMT hstmt, CuTest *tc)
{
	SQLRETURN	rc;

	rc = SQLExecute(hstmt);
	check_error("SQLExecute failed");
	rc = SQLFreeStmt(hstmt, SQL_CLOSE);
	check_error("SQLFreeStmt failed");
}

static void
_prepareQuery(HSTMT hstmt, char *str, CuTest *tc)
{
	SQLRETURN	rc;

	rc = SQLPrepare(hstmt, (SQLCHAR *) str, SQL_NTS);
	check_error("SQLPrepare failed");
}

void Test_odbc_escapes(CuTest *tc)
{
	char outbuf[10];
	rc = connectDB();
	CuAssertTrue(tc, SQL_SUCCEEDED(rc));

	/**** Function escapes ****/

	/* CONCAT */
	prepareQuery(hstmt, "SELECT {fn CONCAT(?, ?) }");
	bindParamString(hstmt, 1, "foo");
	bindParamString(hstmt, 2, "bar");
	executeQuery(hstmt);

	/* LOCATE */
	prepareQuery(hstmt, "SELECT {fn LOCATE(?, ?, 2) }");
	bindParamString(hstmt, 1, "needle");
	bindParamString(hstmt, 2, "this is a needle in an ol' haystack");
	executeQuery(hstmt);

	/* SPACE */
	prepareQuery(hstmt, "SELECT 'x' || {fn SPACE(10) } || 'x'");
	executeQuery(hstmt);

	/**** CALL escapes ****/

	prepareQuery(hstmt, "{ call length(?) }");
	bindParamString(hstmt, 1, "foobar");
	executeQuery(hstmt);

	prepareQuery(hstmt, "{ call right(?, ?) }");
	bindParamString(hstmt, 1, "foobar");
	bindParamString(hstmt, 2, "3");
	executeQuery(hstmt);

	prepareQuery(hstmt, "{ ? = call length('foo') }");
	memset(outbuf, 0, sizeof(outbuf));
	bindOutParamString(hstmt, 1, outbuf, sizeof(outbuf) - 1);
	executeQuery(hstmt);

	/* TODO: This doesn't currently work.
	prepareQuery(hstmt, "{ ? = call concat(?, ?) }");
	memset(outbuf, 0, sizeof(outbuf));
	bindOutParamString(hstmt, 1, outbuf, sizeof(outbuf) - 1);
	bindParamString(hstmt, 2, "foo");
	bindParamString(hstmt, 3, "bar");
	executeQuery(hstmt);
	printf("OUT param: %s\n", outbuf);
	*/

	/**** Date, Time, and Timestamp literals ****/

	prepareQuery(hstmt, "SELECT {d '2014-12-21' } + '1 day'::interval");
	executeQuery(hstmt);

	prepareQuery(hstmt, "SELECT {t '20:30:40' } + '1 hour 1 minute 1 second'::interval");
	executeQuery(hstmt);

	prepareQuery(hstmt, "SELECT {ts '2014-12-21 20:30:40' } + '1 day 1 hour 1 minute 1 second'::interval");
	executeQuery(hstmt);

	disconnectDB();
}
