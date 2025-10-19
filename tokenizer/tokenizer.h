#ifndef TOKENIZER_H
#define TOKENIZER_H

#define MAXVAL 2048

#include <stddef.h>

typedef enum {
    TOKEN_WORD,
    TOKEN_STRING,
    TOKEN_PIPE,
    TOKEN_VAR,
    TOKEN_REDIR_IN,
    TOKEN_REDIR_OUT,
    TOKEN_REDIR_APPEND,
    TOKEN_CHAIN,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_FI,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_DO,
    TOKEN_DONE,
    TOKEN_EOL,
    TOKEN_EOF
}TokenType;

typedef struct Token{
    char *value;
    TokenType type;
}Token;

typedef struct tokens{
    Token **tokens;
    size_t size;
}tokens;

extern size_t curr_tokens_arr_size;
extern tokens tokens_arr;

/*Scanner that goes through the input and splits it to tokens.*/
void Scanner(char * input, tokens *tokens_arr);

#endif