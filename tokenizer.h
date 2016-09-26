#ifndef _TOKENIZER_H
#define _TOKENIZER_H

typedef struct {
	const char *start;
	const char *end;
	const char *eof;
} token;

#define token_length(t) ((t)->end - (t)->start)

#define token_init(t,data,len) do { \
	(t)->start = (data); \
	(t)->end = NULL; \
	(t)->eof = (t)->start + (len); \
} \
while (0)

#define token_is_end(t) ((t)->end == (t)->eof)

int get_token(token *t);
int get_path_token(token *t);
void copy_token_str(char *dest, token *src);

#endif /* _TOKENIZER_H */
