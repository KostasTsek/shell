#ifndef EXEC_H
#define EXEC_H

#include "../AST/ast.h"

/* Forward declarations */
int execute_node(AstNode *node);

/* Execute a command node */
int execute_command(AstNode *node);

/* Execute a pipeline node */
int execute_pipeline(AstNode *node);

/* Execute a sequence node */
int execute_sequence(AstNode *node);

/* Execute an if node */
int execute_if(AstNode *node);

/* Execute a for node */
int execute_for(AstNode *node);

#endif
