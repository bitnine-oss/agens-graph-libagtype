#include "config.h"

#ifndef HAVE_SQL_H
# error Need sql.h!
#endif
#include <sql.h>
#include <sqlext.h>
#include <stdio.h>
#include <string.h>
#include "agtype.h"
#include "agodbc-type.h"

/*
 * Example:
 *
 * struct ag_vertex *v;
 * SQLCHAR *buffer;
 * SQLSMALLINT agType;
 * SQLLEN agRawDataSize;
 * ...
 *
 * SQLExecDirect(hstmt, (SQLCHAR*)"MATCH (n:person '{\"from\": \"Sweden\"}') * RETURN n", SQL_NTS)
 * AG_SQLDescribeCol(hstmt, 1, &agType, &agRawDataSize);
 * buffer = malloc(agRawDataSize);
 *
 * while (SQLFetch(hstmt)) {
 *     AG_SQLGetData(hstmt, 1, agType, (void **)&v, buffer, agRawDataSize, ...);
 *     ag_json_get_string(ag_json_object_get(v->props, "from"));
 * }
 * 
 * ...
 * free(buffer);
 * ag_vertex_free(v);
 */

/* XXX
 * large data ??? 
 */
SQLRETURN
AG_SQLDescribeCol(
		SQLHSTMT StatementHandle,
		SQLUSMALLINT ColumnNumber,
		SQLSMALLINT *DataTypePtr,
		SQLLEN *RawDataSizePtr)
{
#define BUF_LEN 128
	SQLRETURN rc;
    SQLCHAR typeName[BUF_LEN];
	SQLSMALLINT typeNameLen;

	if (DataTypePtr == NULL)
		return SQL_ERROR;

    rc = SQLColAttribute(StatementHandle, ColumnNumber, SQL_DESC_TYPE_NAME, 
						 typeName, BUF_LEN, &typeNameLen, NULL);
	if (rc != SQL_SUCCESS) 
		return rc;
	if (typeNameLen >= BUF_LEN) 
		return SQL_ERROR;

    if (0 == strcmp(typeName, "json") || 0 == strcmp(typeName, "jsonb"))
		*DataTypePtr = AG_PROPERTY;
    else if (0 == strcmp(typeName, "vertex"))
		*DataTypePtr = AG_VERTEX;
    else if (0 == strcmp(typeName, "edge"))
		*DataTypePtr = AG_EDGE;
    else if (0 == strcmp(typeName, "path"))
		*DataTypePtr = AG_PATH;
	else
	{
		*DataTypePtr = AG_NONE;
		return SQL_ERROR;
	}

	if (RawDataSizePtr == NULL)
		return SQL_SUCCESS;

    rc = SQLColAttribute(StatementHandle, ColumnNumber, SQL_DESC_LENGTH, 
						 NULL, 0, NULL, RawDataSizePtr);
	if (rc != SQL_SUCCESS) 
		return rc;

	*RawDataSizePtr += 1; /* null term */

	return SQL_SUCCESS;
}

SQLRETURN 
AG_SQLBindParameter(
		SQLHSTMT        StatementHandle,
		SQLUSMALLINT    ParameterNumber,
		SQLSMALLINT     InputOutputType,
		SQLSMALLINT     AgType,
		SQLPOINTER      ParameterValuePtr,
		SQLLEN         *StrLen_or_IndPtr)
{
	SQLCHAR *data = NULL;
	SQLLEN len = 0;
	if (AgType != AG_PROPERTY)
		return SQL_ERROR;

	if (ParameterValuePtr != NULL)
	{
		data = (char *)ag_json_to_string((ag_json)ParameterValuePtr);
		if (data == NULL)
			return SQL_ERROR;
		else
			len = strlen(data);
	}

	if (len == 0)
		*StrLen_or_IndPtr = SQL_NULL_DATA;

	return SQLBindParameter(StatementHandle, ParameterNumber, InputOutputType,
			SQL_C_CHAR, SQL_CHAR, len, -3802, data, len, StrLen_or_IndPtr);
}

SQLRETURN 
AG_SQLBindCol(
		SQLHSTMT       StatementHandle,
      	SQLUSMALLINT   ColumnNumber,
      	SQLSMALLINT    TargetType,
      	SQLPOINTER    *TargetValuePtr,
      	SQLLEN        *StrLen_or_Ind)
{
	SQLRETURN rc;
	SQLSMALLINT graphoid;
	SQLHDESC hdesc;

	rc = SQLBindCol(StatementHandle, ColumnNumber, SQL_CHAR, 
			(SQLPOINTER)TargetValuePtr, 10, StrLen_or_Ind);	
	if (rc != SQL_SUCCESS)
		return rc;

	rc = SQLGetStmtAttr(StatementHandle, SQL_ATTR_APP_ROW_DESC, &hdesc, 
			SQL_IS_POINTER, NULL);
	if (rc != SQL_SUCCESS)
		return rc;

	switch (TargetType)
	{
	case AG_PROPERTY:
		graphoid = -3802;
		break;
	case AG_VERTEX:
		graphoid = -7012;
		break;
	case AG_EDGE:
		graphoid = -7022;
		break;
	case AG_PATH:
		graphoid = -7032;
		break;
	default:
		return SQL_ERROR;
	}

	rc = SQLSetDescField(hdesc, ColumnNumber, SQL_DESC_SCALE, 
			(SQLPOINTER)&graphoid, 0);
	if (rc != SQL_SUCCESS)
		return rc;

	rc = SQLSetDescField(hdesc, ColumnNumber, SQL_DESC_DATA_PTR, 
			(SQLPOINTER)TargetValuePtr, 0);
	if (rc != SQL_SUCCESS)
		return rc;

	return SQL_SUCCESS;
}

SQLRETURN 
AG_SQLGetData(
		SQLHSTMT StatementHandle, 
		SQLUSMALLINT ColumnNumber, 
		SQLSMALLINT AgType, 
		SQLPOINTER *TargetValue,
		SQLPOINTER BufferPtr,
		SQLLEN BufferSize,
        SQLLEN *StrLen_or_IndPtr)
{
	SQLRETURN rc;
	SQLSMALLINT srcType;

	if (TargetValue == NULL || BufferPtr == NULL)
		return SQL_ERROR;

	rc = AG_SQLDescribeCol(StatementHandle, ColumnNumber, &srcType, NULL);
	if (rc != SQL_SUCCESS)
		return rc;

	if (AgType != srcType)
		return SQL_ERROR;

	rc = SQLGetData(StatementHandle, ColumnNumber, SQL_C_CHAR, BufferPtr, 
			BufferSize, StrLen_or_IndPtr);
	if (rc != SQL_SUCCESS)
		return rc;

	if (*StrLen_or_IndPtr == SQL_NULL_DATA)
		return SQL_SUCCESS;

	if (AgType == AG_PROPERTY) 
        *TargetValue = ag_json_from_string(BufferPtr);
    else if (AgType == AG_VERTEX)
		*TargetValue = ag_vertex_new(BufferPtr);
    else if (AgType == AG_EDGE)
		*TargetValue = ag_edge_new(BufferPtr);
    else if (AgType == AG_PATH)
		*TargetValue = ag_path_new(BufferPtr);
    else /* Not supported type */
		return SQL_ERROR;

	if (*TargetValue == NULL) /* decoding error */
		return SQL_ERROR;

	return SQL_SUCCESS;
}

