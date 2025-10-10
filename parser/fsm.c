#include "fsm.h"
#include "../utils/utils.h"

state HandleAssignment(char **assignments, int *index, char **splitted_input){
    assignments[*index] = (char*)safe_alloc(sizeof(char) * LINE_MAX);
	strcpy(assignments[*index++], splitted_input[i++]);
	curr_state = START;
}