#include <string.h>
#include "tokenizer.h"

static const char *
get_props(token *tok)
{
	const char *p = tok->start;
	int depth = 0;

	while (p != tok->eof)
	{
		if (depth == 1 && *p == '}')
			return p;
		else if (*p == '}')
			depth--;
		else if (*p == '{') 
		{
			depth++;
		}
		p++;
	}
	return NULL;
}

static const char *
tok_strchr(token *tok, char c)
{
	const char *p = tok->start;

	while (p != tok->eof)
	{
		if (*p == c)
			return p;
		p++;
	}
	return NULL;
}

/*
 * 3 token types:
 *   1. id -> label
 *   2. [] -> gid
 *   3. {} -> json string
 */
int
get_token(token *data)
{
	int depth = 0;

	if (token_is_end (data))
		return 0;

	if (data->end != NULL)
		data->start = data->end;

	switch (*data->start) 
	{
		case '[': 
			data->end = tok_strchr(data, ']') + 1;
			break;
		case '{':
			data->end = get_props(data) + 1;
			break;
		default:
			data->end = tok_strchr(data, '[');
			break;
	}
	return 1;
}

void
copy_token_str(char *dest, token *src)
{
	strncpy (dest, src->start, token_length(src));
	dest[token_length(src)] = '\0';
}

/*
 * vertex0,edge0,vertex1,edge1,vertex2
 */
int
get_path_token(token *data)
{
	int depth = 0;
	int in_gid = 0;
	const char *p;

	if (token_is_end (data))
		return 0;

	if (data->end != NULL)
		data->start = data->end + 1; /* skip "," */

	p = data->start;
	while (p != data->eof)
	{
		char c = *p;
		if ('{' == c) 
		{
			depth++;
		}
		else if ('}' == c) 
		{
			depth--;
		}
		else if (0 == depth && '[' == c) 
		{ 
			in_gid = 1;
		}
		else if (in_gid && ']' == c) 
		{
			in_gid = 0;
		}
		else if (0 == depth && !in_gid && ',' == c) 
		{
			data->end = p;
			return 1;
		}
		p++;
	}
	
	data->end = p;
	return 1;
}
