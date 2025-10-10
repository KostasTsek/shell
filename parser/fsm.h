#ifndef FSM_H
#define FSM_H

#include "../utils/utils.h"

typedef enum {
	START,
	VAR,
	CMD,
	PARAM,
	REDIR
}state;

struct state_indexes{
    int commandIndex;
    int parametersIndex;
    int assignmentsIndex;
    int redirectionsIndex;
}StateIndexes;

typedef struct Variable{
	char *name;
	char *value;
}Variable;


void fsm(char **splitted_input, char command[][LINE_MAX], char ***parameters, char **assignments, char **redirections, Variable *var_hash_table);

/*State handlers*/
state HandleAssignment(char **assignments, int index, char **splitted_input);
state HandleCommand(char command[][LINE_MAX], int index, char **splitted_input);

#endif