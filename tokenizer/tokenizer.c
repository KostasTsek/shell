#include "tokenizer.h"
#include "../utils/utils.h"
#include <string.h>
#include <stdio.h>

typedef enum states{
    START,
    WORD,
    SEP_CHAR,
    APPEND,
    STRING,
    END
}State;

size_t curr_tokens_arr_size = 2048;

tokens tokens_arr;

TokenType GetStrTok(const char * tok_val);
void emit(TokenType type, tokens * tokens_arr, const char * val);
State FSM(const char c, State curr_state, char * val, tokens * tokens_arr);

char isWhiteSpace(char c);
char isSeperatingChar(char c);

void Scanner(char * input, tokens *tokens_arr){
    int i = 0;
    
    tokens_arr->tokens = (Token **)safe_alloc(sizeof(Token *) * curr_tokens_arr_size);
    char * curr_val = (char *)safe_alloc(sizeof(char *) * MAXVAL);
    State curr_state = START;

    while(curr_state != END){
        curr_state = FSM(input[i], curr_state, curr_val, tokens_arr);
        if(curr_state == START || curr_state == APPEND || curr_state == STRING) i++;
    }
}

State FSM(const char c, State curr_state, char * val, tokens * tokens_arr){
    switch (curr_state){
    case START:
        if(isWhiteSpace(c)){
            curr_state = START;
            if(strcmp(val, "")) emit(GetStrTok(val), tokens_arr, val);
            strcpy(val, "");
        }
        else if(isSeperatingChar(c))
            curr_state = SEP_CHAR;
        else if(c == '\0' || c == EOF){
            curr_state = END;
            emit(GetStrTok(val), tokens_arr, val);
            strcpy(val, "");
            emit(TOKEN_EOF, tokens_arr, "");
        }
        else
            curr_state = WORD;
        break;
    case SEP_CHAR:
        if(!strcmp(val, "")){
            append_char(val, c);
        }
        else{
            emit(GetStrTok(val), tokens_arr, val);
            strcpy(val, "");
            append_char(val, c);
        }

        if(c == '>')
            curr_state = APPEND;
        else{
            curr_state = START;
            emit(GetStrTok(val), tokens_arr, val);
            strcpy(val, "");
        }
        break;
    case APPEND:
        if(c == '>')
            append_char(val, '>');
        emit(GetStrTok(val), tokens_arr, val);
        strcpy(val, "");
        curr_state = START;
        break;
    case WORD:
        append_char(val, c);
        if(c == '"')
            curr_state = STRING;
        else
            curr_state = START;
        break;
    case STRING:
        append_char(val, c);
        if(c == '"')
            curr_state = START;
        break;
    default:
        break;
    }
    return curr_state;
}

TokenType GetStrTok(const char * tok_val){
    if(!strcmp(tok_val, ";"))
        return TOKEN_CHAIN;
    else if(!strcmp(tok_val, "|"))
        return TOKEN_PIPE;
    else if(!strcmp(tok_val, ">"))
        return TOKEN_REDIR_OUT;
    else if(!strcmp(tok_val, ">>"))
        return TOKEN_REDIR_APPEND;
    else if(!strcmp(tok_val, "<"))
        return TOKEN_REDIR_IN;
    else if(!strcmp(tok_val, "\n"))
        return TOKEN_EOL;
    else if(!strcmp(tok_val, "if"))
        return TOKEN_IF;
    else if(!strcmp(tok_val, "then"))
        return TOKEN_THEN;
    else if(!strcmp(tok_val, "fi"))
        return TOKEN_FI;
    else if(!strcmp(tok_val, "for"))
        return TOKEN_FOR;
    else if(!strcmp(tok_val, "in"))
        return TOKEN_IN;
    else if(!strcmp(tok_val, "do"))
        return TOKEN_DO;
    else if(!strcmp(tok_val, "done"))
        return TOKEN_DONE;
    
    else if(strchr(tok_val, '$'))
        return TOKEN_VAR;
    else
        return TOKEN_WORD;
}

void emit(TokenType type, tokens * tokens_arr, const char * val){
    size_t index = 0;
    for(int i = 0; tokens_arr->tokens[i] != NULL && i < curr_tokens_arr_size; i++)
        index++;
    if(index >= tokens_arr->size){
        curr_tokens_arr_size = curr_tokens_arr_size * 2;
        tokens_arr->tokens = (Token **)safe_realloc(tokens_arr->tokens, sizeof(Token *) * curr_tokens_arr_size);
        tokens_arr->size = curr_tokens_arr_size;
    }

    tokens_arr->tokens[index] = (Token *)safe_alloc(sizeof(Token));
    tokens_arr->tokens[index]->type = type;
    tokens_arr->tokens[index]->value = (char *)safe_alloc(sizeof(char) * MAXVAL);
    strcpy(tokens_arr->tokens[index]->value, val); 
}

char isWhiteSpace(char c){
    return (c == ' ' || c == '\t');
}

char isSeperatingChar(char c){
    return (c == ';' || c == '|' || c == '>' || c == '<' || c == '\n');
}
