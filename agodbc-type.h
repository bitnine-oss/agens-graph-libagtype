#ifndef _AGODBC_TYPE_H
#define _AGODBC_TYPE_H

/* graph types */

#define AG_NONE     -1
#define AG_PROPERTY  0
#define AG_VERTEX    1
#define AG_EDGE      2
#define AG_PATH      3

SQLRETURN AG_SQLDescribeCol(
		SQLHSTMT StatementHandle,
		SQLUSMALLINT ColumnNumber,
		SQLSMALLINT *DataTypePtr,
		SQLLEN *CharSizePtr);

SQLRETURN AG_SQLBindParameter(
		SQLHSTMT        StatementHandle,
		SQLUSMALLINT    ParameterNumber,
		SQLSMALLINT     InputOutputType,
		SQLSMALLINT     AgType,
		SQLPOINTER      ParameterValuePtr,
		SQLLEN         *StrLen_or_IndPtr);

SQLRETURN AG_SQLBindCol(
		SQLHSTMT       StatementHandle,
      	SQLUSMALLINT   ColumnNumber,
      	SQLSMALLINT    TargetType,
      	SQLPOINTER    *TargetValuePtr,
      	SQLLEN        *StrLen_or_Ind);

SQLRETURN AG_SQLGetData(
		SQLHSTMT StatementHandle, 
		SQLUSMALLINT ColumnNumber, 
		SQLSMALLINT AgType, 
		SQLPOINTER *TargetValue,
		SQLPOINTER BufferPtr,
		SQLLEN BufferSize,
        SQLLEN *StrLen_or_IndPtr);

#endif /* _AGODBC_TYPE_H */
