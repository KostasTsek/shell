#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "../utils/utils.h"

/* Commands functions */
void Add_Assignment(Token tok, Assignment ** assignment);
void Add_Redirection(Token tok, RedirType redir_type, Redirection ** redirection);
void Add_Argv(Token tok, char *** args, int arg_pos, int * args_size);

char isCommandTerminator(Token tok);
char isKeyWord(Token tok);
char isRedirection(Token tok);
RedirType Redir_Mapper(Token tok);

AstNode *ParseCommand(tokens tokens, int * pos){
    AstNode *node = (AstNode *)safe_alloc(sizeof(AstNode));
    node->type = NODE_COMMAND;
    node->command.assignments = NULL;
    node->command.redirections = NULL;
    node->command.argv = NULL;
    int arg_pos = 0, args_size = 248;


    while(tokens.tokens[*pos]->type != TOKEN_EOF && !isCommandTerminator(*tokens.tokens[*pos])){
        if(isRedirection(*tokens.tokens[*pos])){
            RedirType curr_type = Redir_Mapper(*tokens.tokens[*pos]);
            (*pos)++;
            Add_Redirection(*tokens.tokens[(*pos)++], curr_type, &node->command.redirections);
        }
        else{
            if(strchr(tokens.tokens[*pos]->value, '=') && node->command.argv == NULL){
                Add_Assignment(*tokens.tokens[(*pos)++], &node->command.assignments);
            }
            else{
                Add_Argv(*tokens.tokens[(*pos)++], &node->command.argv, arg_pos++, &args_size);
            }
        }
    }
    return node;
}

AstNode *ParsePipeline(tokens tokens, int * pos){
    AstNode *pipeNode = (AstNode *)safe_alloc(sizeof(AstNode));
    pipeNode->type = NODE_PIPELINE;
    pipeNode->pipeline.size = 128;
    pipeNode->pipeline.commands = (AstNode **)safe_alloc(sizeof(AstNode *) * pipeNode->pipeline.size);
    pipeNode->pipeline.count = 0;

    while(tokens.tokens[*pos]->type != TOKEN_EOF && (tokens.tokens[*pos]->type == TOKEN_PIPE || 
    !isCommandTerminator(*tokens.tokens[*pos]))){
        if (tokens.tokens[*pos]->type == TOKEN_PIPE) (*pos)++;
        if(pipeNode->pipeline.count >= pipeNode->pipeline.size){
            pipeNode->pipeline.size *= 2;
            pipeNode->pipeline.commands = (AstNode **)safe_realloc(pipeNode->pipeline.commands, sizeof(AstNode *) * pipeNode->pipeline.size);
        }
        pipeNode->pipeline.commands[pipeNode->pipeline.count++] = ParseCommand(tokens, pos);
    }
    return pipeNode;
}

AstNode *ParseSequence(tokens tokens, int * pos){
    AstNode *Seq_Node = (AstNode *)safe_alloc(sizeof(AstNode));
    Seq_Node->type = NODE_SEQUENCE;
    Seq_Node->sequence.right = NULL;

    if(tokens.tokens[*pos]->type == TOKEN_IF)
        Seq_Node->sequence.left = ParseIf(tokens, pos);
    else if(tokens.tokens[*pos]->type == TOKEN_FOR)
        Seq_Node->sequence.left = ParseFor(tokens, pos);
    else
        Seq_Node->sequence.left = ParsePipeline(tokens, pos);

    if(tokens.tokens[*pos]->type == TOKEN_CHAIN){
        (*pos)++;
        Seq_Node->sequence.right = ParseSequence(tokens, pos);
    }
    
    return Seq_Node;
}

AstNode *ParseIf(tokens tokens, int * pos){
    AstNode *ifNode = (AstNode *)safe_alloc(sizeof(AstNode));
    ifNode->type = NODE_IF;

    (*pos)++;
    ifNode->if_stmt.condition = ParseSequence(tokens, pos);

    if(!isKeyWord) (*pos)++;
    
    if(tokens.tokens[(*pos)++]->type != TOKEN_THEN){
        perror("Syntax error: No then block.\n");
        return NULL;
    }
    if(tokens.tokens[*pos]->type == TOKEN_EOL) (*pos)++;
    else if(isCommandTerminator(*tokens.tokens[*pos])){
        fprintf(stderr, "Syntax error near unexpected token %s\n", tokens.tokens[*pos]->value);
        return NULL;
    }

    ifNode->if_stmt.then_branch = ParseSequence(tokens, pos);
    
    if(!isKeyWord) (*pos)++;
    if(tokens.tokens[*pos]->type != TOKEN_FI){
        if(tokens.tokens[*pos]->type == TOKEN_EOF || tokens.tokens[*pos]->type == TOKEN_EOL) fprintf(stderr, "Syntax error near end of file\n");
        else fprintf(stderr, "Syntax error near unexpected token %s\n\n", tokens.tokens[*pos]->value);
        return NULL;
    }
    (*pos)++;
    return ifNode;
}

AstNode *ParseFor(tokens tokens, int * pos){
    AstNode *forNode = (AstNode *)safe_alloc(sizeof(AstNode));
    forNode->type = NODE_FOR;
    forNode->forloop.var_name = (char *)safe_alloc(sizeof(char) * MAXVAL);
    forNode->forloop.items = (char **)safe_alloc(sizeof(char *) * MAXVAL);

    (*pos)++;

    strcpy(forNode->forloop.var_name, tokens.tokens[(*pos)++]->value);
    
    if(tokens.tokens[*pos]->type != TOKEN_IN){
        fprintf(stderr, "Syntax error near unexpected token '%s'\n", tokens.tokens[*pos]->value);
        return NULL;
    }
    (*pos)++;
    int curr_item = 0;
    while(tokens.tokens[*pos]->type != TOKEN_DO && !isCommandTerminator(*tokens.tokens[*pos])){
        forNode->forloop.items[curr_item] = (char *)safe_alloc(sizeof(char *) * MAXVAL);
        strcpy(forNode->forloop.items[curr_item], tokens.tokens[*pos]->value);
        curr_item++;
        (*pos)++;
    }

    if(!isKeyWord(*tokens.tokens[*pos])) (*pos)++;

    if(tokens.tokens[*pos]->type != TOKEN_DO){
        fprintf(stderr, "Syntax error near unexpected token '%s'\n", tokens.tokens[*pos]->value);
        return NULL;
    }

    (*pos)++;
    if(isCommandTerminator(*tokens.tokens[*pos])) (*pos)++;

    forNode->forloop.body = ParseSequence(tokens, pos);
    
    if(!isKeyWord(*tokens.tokens[*pos]))(*pos)++;

    if(tokens.tokens[*pos]->type != TOKEN_DONE){
        perror("Syntax error: unexpexted end of file.\n");
        return NULL;
    }

    return forNode;
}

/* Adds an assignment to the assignments list of a command. */
void Add_Assignment(Token tok, Assignment ** assignment){
    int i = 0;
    Assignment *new_assignment = (Assignment *)safe_alloc(sizeof(Assignment));
    
    /* Spliting assignment to var name and value. */
    new_assignment->value = (char*)safe_alloc(sizeof(char) * MAXVAL);
    new_assignment->var_name = (char*)safe_alloc(sizeof(char) * MAXVAL);

    for(i = 0; tok.value[i] != '='; i++){
        new_assignment->var_name[i] = tok.value[i];
    }
    new_assignment->var_name[i] = '\0';

    for(i = i+1; tok.value[i] != '\0'; i++){
        new_assignment->value[i] = tok.value[i];
    }
    new_assignment->value[i] = tok.value[i];

    if(*assignment == NULL)
        *assignment = new_assignment;
    else{
        Assignment *curr = *assignment;
        while (curr->next != NULL) curr = curr->next;
        curr->next = new_assignment;
    }
}

/* Adds a redirection in the redirections list of a command. */
void Add_Redirection(Token tok, RedirType redir_type, Redirection ** redirection){
    Redirection * new_redir = (Redirection *)safe_alloc(sizeof(Redirection));
    new_redir->redir_type = redir_type;
    new_redir->target = (char*)safe_alloc(sizeof(char) * MAXVAL);
    strcpy(new_redir->target, tok.value);

    if(*redirection == NULL)
        *redirection = new_redir;
    else{
        Redirection * curr_redir = *redirection;
        while(curr_redir->next != NULL) curr_redir = curr_redir->next;
        curr_redir->next = new_redir;
    }
}

/* Adds an argument to a command's argument array. */
void Add_Argv(Token tok, char *** args, int arg_pos, int * args_size){
    if(*args == NULL){
        *args = (char **)safe_alloc(sizeof(char *) * (*args_size));
    }
    else if(arg_pos >= *args_size){
        *args_size = *args_size * 2;
        *args = (char **)safe_realloc(args, sizeof(char *) * (*args_size));
    }

    (*args)[arg_pos] = (char *)safe_alloc(sizeof(char) * MAXVAL);
    strcpy((*args)[arg_pos], tok.value);
}

/* BOOL FUNCTIONS */

char isCommandTerminator(Token tok){
    char expr = (tok.type != TOKEN_WORD && tok.type != TOKEN_REDIR_APPEND &&
        tok.type != TOKEN_REDIR_OUT && tok.type != TOKEN_REDIR_IN && tok.type != TOKEN_STRING); 
    return expr;
}

char isKeyWord(Token tok){
    return (tok.type == TOKEN_IF || tok.type == TOKEN_THEN || tok.type == TOKEN_FI ||
    tok.type == TOKEN_DO || tok.type == TOKEN_FOR || tok.type == TOKEN_DONE);
}

char isRedirection(Token tok){
    return (tok.type == TOKEN_REDIR_APPEND || tok.type == TOKEN_REDIR_IN || tok.type == TOKEN_REDIR_OUT);
}

RedirType Redir_Mapper(Token tok){
    if(tok.type == TOKEN_REDIR_APPEND)
        return APPEND;
    else if(tok.type == TOKEN_REDIR_IN)
        return IN;
    else if(tok.type == TOKEN_REDIR_OUT)
        return OUT;
    
    perror("Not a redirection.. Parser failed, closing terminal\n");
    exit(EXIT_FAILURE);
}