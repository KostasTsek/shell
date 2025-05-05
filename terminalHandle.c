#include "terminalHandle.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

/*Hidden functions.*/
int HashFunction(char *var_name);

/*Prints out the terminal.*/
void Command_Prompt(void){
	char curr_dir[1024];
	char *user = getenv("USER");
	const char *AM = "csd5260";
	
	if(getcwd(curr_dir, sizeof(curr_dir)) != NULL){
		printf("%s-hy345sh@%s:%s$ ", AM, user, curr_dir);		
	}
	else{
		perror("getcwd() error\n");
	}
}

/*Splits the user's input to the correct assignment.*/
void Read_Prompt(char *command, char **parameters, char **assignments, char **redirections, Variable *var_hash_table){
	/*Counters*/
	int i = 0, j = 0, assignment_index = 0, params_index = 1, redirs_index = 0, in_index = 0;
	
	char input[LINE_MAX];
	char splitted_input[LINE_MAX][LINE_MAX] = {0};
	state curr_state = START;
	
	fgets(input, LINE_MAX, stdin);
	
	while(input[in_index] != '\n'){
		in_index++;
	}
	input[in_index] = '\0';
	
	/*Splitting the input and saving it to splitted_input.*/
	in_index = 0;
	while(input[in_index] != '\0'){
		
		while(input[in_index] == ' ') in_index++;
		
		if(input[in_index] == '>'){
			if(input[in_index + 1] == '>'){
				splitted_input[i][j++] = input[in_index++];
				splitted_input[i][j++] = input[in_index++];
			}
			else{
				splitted_input[i][j++] = input[in_index++];
			}
			splitted_input[i][j] = '\0';
			i++;
			j = 0;
		}
		else if(input[in_index] == '<'){
			splitted_input[i][j++] = input[in_index++];
			splitted_input[i][j] = '\0';
			i++;
			j = 0;
		}
		else{
			if(input[in_index + 1] != ' ' && input[in_index + 1] != '\0' && input[in_index + 1] != '>' && input[in_index + 1] != '<')
				splitted_input[i][j++] = input[in_index++];
			else{
				splitted_input[i][j++] = input[in_index++];
				splitted_input[i][j] = '\0';
				i++;
				j=0;
			}
		}
	}
	
	
	/*Using fsm in order to understand whether the user wants to execute a command or set a variable and
	organizing everything to their arrays.*/
	i = 0;
	while(strcmp(splitted_input[i], "") != 0){
		switch(curr_state){
			case START:
				if(strchr(splitted_input[i], '=') != NULL && isVariableCheck(splitted_input[i]))
					curr_state = VAR;
				else if(strchr(splitted_input[i], '<') != NULL || strchr(splitted_input[i], '>') != NULL || strstr(splitted_input[i], ">>") != NULL)
					curr_state = REDIR;
				else
					curr_state = CMD;
				break;
			case VAR:
				assignments[assignment_index] = (char *)malloc(sizeof(char) * LINE_MAX);
				if(assignments[assignment_index] == NULL){
					perror("Malloc error\n");
					exit(EXIT_FAILURE);
				}	
				strcpy(assignments[assignment_index++], splitted_input[i++]);
				curr_state = START;
				break;
			case CMD:
				if(strcmp(command, "") == 0){
					strcpy(command, splitted_input[i++]);
				}
				curr_state = PARAM;
				break;
			case PARAM:
				if(parameters[0] == NULL){
					parameters[0] = (char *)malloc(sizeof(char) * LINE_MAX);
					if(parameters[0] == NULL){
						perror("Malloc error\n");
						exit(EXIT_FAILURE);
					}	
					strcpy(parameters[0], command);
				}
				if(strchr(splitted_input[i], '<') != NULL || strchr(splitted_input[i], '>') != NULL || strstr(splitted_input[i], ">>") != NULL){
					curr_state = REDIR;
					break;
				}
				else{
					parameters[params_index] = (char *)malloc(sizeof(char) * LINE_MAX);
					if(parameters[params_index] == NULL){
						perror("Malloc error\n");
						exit(EXIT_FAILURE);
					}
					if(strchr(splitted_input[i], '$') != NULL)
						strcpy(parameters[params_index++], Get_Variable(splitted_input[i++], var_hash_table));
					else
						strcpy(parameters[params_index++], splitted_input[i++]);
				}
				break;
			case REDIR:
				if(redirections[redirs_index] == NULL){
					redirections[redirs_index] = (char *)malloc(sizeof(char) * LINE_MAX);
					if(redirections[redirs_index] == NULL){
						perror("Malloc error\n");
						exit(EXIT_FAILURE);
					}
				}
				strcpy(redirections[redirs_index], splitted_input[i++]);
				if(splitted_input[i][0] == '<' ||splitted_input[i][0] == '>'){
					strcat(redirections[redirs_index], " ");
					strcat(redirections[redirs_index++], splitted_input[i++]);
				}
				else{
					strcat(redirections[redirs_index++], splitted_input[i++]);
				}
				curr_state = START;
				break;
			default:
				break;
		}
	}
}

/*Nullifying the arrays and reseting command string to empty.*/
void Reset_Holders(char *command, char **parameters, char **assignments, char **redirections){
	int i;
	
	for(i = 0; parameters[i] != NULL; i++){
		free(parameters[i]);
		parameters[i] = NULL;
	}
	for(i = 0; assignments[i] != NULL; i++){
		free(assignments[i]);
		assignments[i] = NULL;
	}
	
	for(i = 0; redirections[i] != NULL; i++){
		free(redirections[i]);
		redirections[i] = NULL;
	}
	
	strcpy(command, "");
}

/*Returns wether or not a variable's name is correct.*/
int isVariableCheck(char *variable){
	int i = 0, j;
	char *curr_addr;
	
	char foul_input[25] = "-{}+/*#<>?[].|^@!()&%,\"\'";
	
	char var_name[LINE_MAX] = "";
	
	for(curr_addr = variable; curr_addr < strchr(variable, '='); curr_addr++){
		var_name[i++] = *curr_addr;
	}
	var_name[i] = '\0';
	
	if(!isalpha(var_name[0]) || strcmp(var_name, "") == 0){
		return 0;
	}
	for(i = 0; var_name[i] != '\0'; i++){
		for(j = 0; j < 25; j++){
			if(var_name[i] == foul_input[j])
				return 0;
		}
	}
	
	return 1;
}

/*Set's the variable to the Hash table.*/
void Set_Variable(char **assignment, Variable *var_hash_table){
	char var_name[LINE_MAX];
	char var_value[LINE_MAX];
	char *curr_addr;
	int index = 0;
	
	int i = 0, j;
	
	for(j = 0; assignment[j] != NULL; j++){
		i=0;
		for(curr_addr = assignment[j]; curr_addr < strchr(assignment[j], '='); curr_addr++){
			var_name[i++] = *curr_addr;
		}
		var_name[i] = '\0';
		
		i = 0;
		for(curr_addr = strchr(assignment[j], '=') + 1; *curr_addr != '\0'; curr_addr++){
			var_value[i++] = *curr_addr;
		}
		var_value[i] = '\0';
		
		index = HashFunction(var_name);
		
		if(var_hash_table[index].name == NULL)
			var_hash_table[index].name = (char *)malloc(sizeof(char) * LINE_MAX);
		if(var_hash_table[index].value == NULL)
			var_hash_table[index].value = (char *)malloc(sizeof(char) * LINE_MAX);
		strcpy(var_hash_table[index].name, var_name);
		strcpy(var_hash_table[index].value, var_value);
	}
}

/*Get's the variable's value from the hash table given it's name.*/
char *Get_Variable(char *var_cmd, Variable *var_hash_table){
	char var_name[LINE_MAX] = "";
	
	int index = 0;
	
	strcpy(var_name, var_cmd + 1);
	
	index = HashFunction(var_name);
	
	if(var_hash_table[index].value != NULL && strcmp(var_hash_table[index].name, var_name) == 0)
		return var_hash_table[index].value;
	
	return "";
}

/*Sets the correct file descriptors to the correct file ids based on the redirection the user wants to do.*/
void Redir_Handler(char **redirections){
	char *filename;
	
	for(int i = 0; redirections[i] != NULL; i++){
		if(strchr(redirections[i], ' ') != NULL){
			printf("hy345sh: Unexpected token\n");
			exit(EXIT_FAILURE);
		}
		
		if(strstr(redirections[i], ">>")){
			filename = redirections[i] + 2;
			int file = open(filename, O_WRONLY | O_APPEND | O_CREAT, 0777);
			if(file == -1){
				perror("opening file error");
				return;
			}
			dup2(file, STDOUT_FILENO);
			close(file);
		}
		else if(strchr(redirections[i], '>')){
			filename = redirections[i] + 1;
			int file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
			if(file == -1){
				perror("opening file error");
				return;
			}
			dup2(file, STDOUT_FILENO);
			close(file);
		}
		else{
			filename = redirections[i] + 1;
			int file = open(filename, O_RDONLY);
			if(file < 0){
				if (errno == ENOENT) {
           				 printf("Error: The file '%s' does not exist.\n", filename);
       				} 
       				else {
            				perror("opening file error");
            				
        			}
        			return;
			}
			dup2(file, STDIN_FILENO);
			close(file);
		}
	}
}

/*Local Function implementations.*/

/*Simple Hash function takes the variable's name and converts it to an sum of integers multyplied by 31.*/
int HashFunction(char *var_name){
	int result = 0;
	const int prime_multiplier = 31;
	
	for(int i = 0; var_name[i] != '\0'; i++){
		result = result * prime_multiplier + var_name[i];
	}
	
	return result < 0 ? (result * -1) % HASH_TABLE_SLOTS : result % HASH_TABLE_SLOTS; 
}

