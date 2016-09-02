#include "config.h"

#ifndef HAVE_SQL_H
# error Need sql.h!
#endif
#include <sql.h>

#include "agtype.h"

#if 0
SQLRETURN 
AG_SQLGetData(StatementHandle, SQLUSMALLINT ColumnNumber,
              SQLSMALLINT TargetType, SQLPOINTER *TargetValue,
              SQLLEN *StrLen_or_Ind)
{
    SQLLEN type;
    SQLCHAR typeName[];

    while (SQLGetData(StatementHandle, ColumnNumber, SQL_C_CHAR, buffer)) {
        string_append(data, buffer);
    }

    SQLColAttribute(StatementHandle, ColumnNumber, SQL_DESC_TYPE_NAME, typeName ...);
    if (TargetType == AG_PROPERTY && typeName == "json")
        *TargetValue = ag_value_from_json_string(data);
    else if (TargetType == AG_VERTEX && typeName == "vertex")
		*TargetValue = ag_vertex_new(data);
    else if (TargetType == AG_EDGE && typeName == "edge")
		*TargetValue = ag_edge_new(data);
    else if (TargetType == AG_PATH && typeName == "path")
		*TargetValue = ag_path_new(data);
    else
	{
		/* Not supported type */
		return SQL_ERROR;
	}

	return SQL_SUCCESS;
#endif
}
~ 
