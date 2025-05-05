#define ARG_MAX 128
#define LINE_MAX 2048
#define HASH_TABLE_SLOTS 1021

typedef enum {
	START,
	VAR,
	CMD,
	PARAM,
	REDIR
}state;

typedef struct Variable{
	char *name;
	char *value;
}Variable;

/*Retrieves the user and directory the user is at and displays them in the terminal.*/
void Command_Prompt(void);

/*Splits user input to command and parameters.
* command is the program the user wants to run.
* parameters is an array that the user gives to the program.
*/
void Read_Prompt(char *command, char **parameters, char **assignments, char **redirections, Variable *var_hash_table);

/*Frees every allocated address on parameters and assignments arrays and sets command to the empty array "".*/
void Reset_Holders(char *command, char **parameters, char **assignments, char **redirections);

/*Sets the variable in the hash table.*/
void Set_Variable(char **assignment, Variable *var_hash_table);

/*Gets the value of the given variable from the hash table.*/
char *Get_Variable(char *var_cmd, Variable *var_hash_table);

/*Checks if the given assignment is valid.*/
int isVariableCheck(char *variable);

/*Handles redirections in redirections array.*/
void Redir_Handler(char **redirections);

