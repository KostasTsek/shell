#include "terminalHandle.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

//int commands_len(char ***parameters);

int main(void){
	
	char command[ARG_MAX][LINE_MAX] = {0};
	char ***parameters;
	char **assignments;
	char **redirections;
	
	Variable var_hash_table[HASH_TABLE_SLOTS];
	
	parameters = (char ***)malloc(sizeof(char *) * ARG_MAX);
	if(parameters == NULL){
		perror("Malloc error\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < ARG_MAX; i++){
		parameters[i] = (char **)malloc(sizeof(char *) * ARG_MAX);
		if(parameters == NULL){
			perror("Malloc error\n");
			exit(EXIT_FAILURE);
		}	
	}
	assignments = (char **)malloc(sizeof(char *) * LINE_MAX);
	if(assignments == NULL){
		perror("Malloc error\n");
		exit(EXIT_FAILURE);
	}
	redirections = (char **)malloc(sizeof(char *) * LINE_MAX);
	if(redirections == NULL){
		perror("Malloc error\n");
		exit(EXIT_FAILURE);
	}
	
	while(1){
		Command_Prompt();
		Read_Prompt(command, parameters, assignments, redirections, var_hash_table);
		if(redirections[0] != NULL || strcmp(command[0], "") != 0){
			for(int i = 0; i < commands_len; i++){
				char bash_name[LINE_MAX] = "hy345: ";
				const char * message = strcat(bash_name, command[i]);
				if(strcmp(command[i], "cd") == 0){
					if(chdir(parameters[i][1])){
						perror(message);
					}
				}
				else if(strcmp(command[i], "exit") == 0){
					return 0;
				}
				else{
					int pid = fork();
					if(pid == 0){
						if(redirections[0] != NULL){
							Redir_Handler(redirections);
						}
						if(strcmp(command[i], "") != 0){
							execvp(command[i], parameters[i]);
							perror(message);
							exit(EXIT_FAILURE);
						}
						else{
							exit(EXIT_FAILURE);
						}
					}
					else if(pid > 0){
						int status;
					
						waitpid(-1, &status, 0);
					}
					else{
						perror("Fork Failed\n");
					}
				}
			}
		}
		else{
			Set_Variable(assignments, var_hash_table);	
		}
		
			Reset_Holders(command, parameters, assignments, redirections);
		}
	return 0;
}

/*int commands_len(char ***parameters){
	int len = 1;
	for(int i = 0; i < ARG_MAX; i++){
		if(parameters[i][1] == NULL)
			return len;
		len++;
	}
	return len;
}*/
