#ifndef AST_H
#define AST_H

#include "../tokenizer/tokenizer.h"

typedef enum{
    NODE_COMMAND,
    NODE_PIPELINE,
    NODE_SEQUENCE,
    NODE_IF,
    NODE_FOR
} NodeType;

typedef enum{
    IN,
    OUT,
    APPEND
}RedirType;

typedef struct redirection{
    RedirType redir_type;
    char *target;
    struct redirection *next;
}Redirection;

typedef struct assignment{
    char * var_name;
    char * value;
    struct assignment *next;
}Assignment;

typedef struct astnode{
    NodeType type;

    union{
        struct {
            char **argv;
            Assignment *assignments;
            Redirection *redirections;
        } command;
        
        struct{
            struct astnode **commands;
            size_t count;
            size_t size;
        } pipeline;

        struct{
            struct astnode *left;
            struct astnode *right;
        } sequence;

        struct{
            struct astnode *condition;
            struct astnode *then_branch;
        }if_stmt;

        struct{
            char *var_name;
            char **items;
            struct astnode *body;
        } forloop;
        
    };

}AstNode;

AstNode *ParseCommand(tokens tokens, int * pos);
AstNode *ParsePipeline(tokens tokens, int * pos);
AstNode *ParseSequence(tokens tokens, int * pos);
AstNode *ParseIf(tokens tokens, int * pos);
AstNode *ParseFor(tokens tokens, int * pos);

#endif