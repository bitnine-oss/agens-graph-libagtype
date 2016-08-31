#ifndef _TOKENIZER_H
#define _TOKENIZER_H

typedef struct {
	char *start;
	char *end;
} token;

#define token_length(t) ((t)->end - (t)->start)

#define token_init(t,data) do { \
	(t)->start = (data); \
	(t)->end = NULL; \
} \
while (0)

#define token_is_end(t) ((t)->end != NULL && *(t)->end == '\0')

int get_token(token *t);
int get_path_token(token *t);
void copy_token_str(char *dest, token *src);

#endif /* _TOKENIZER_H */
