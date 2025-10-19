#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tokenizer/tokenizer.h"
#include "AST/ast.h"
#include "executor/executor.h"

char* read_input(){
    char *buffer = NULL;
    size_t bufsize = 0;
    if(getline(&buffer, &bufsize, stdin) == -1){
        if(feof(stdin)){
            exit(0); // Ctrl+D
        } else {
            perror("getline failed");
            exit(EXIT_FAILURE);
        }
    }
    return buffer;
}

int main() {
    while(1){
        char curr_dir[1024];
	    char *user = getenv("USER");
	    const char *AM = "csd5260";
        
	    if(getcwd(curr_dir, sizeof(curr_dir)) != NULL){
	    	printf("%s-hy345sh@%s:%s$ ", AM, user, curr_dir);
            fflush(stdout);		
	    }
	    else{
	    	perror("getcwd() error\n");
	    }

        char *input = read_input();
        if(input[strlen(input)-1] == '\n')
            input[strlen(input)-1] = '\0'; // remove trailing newline

        /* Tokenize input */
        tokens toks;
        toks.size = 2048;
        toks.tokens = NULL;
        Scanner(input, &toks);

        int pos = 0;
        AstNode *ast_root = ParseSequence(toks, &pos);
        if(!ast_root){
            fprintf(stderr, "Parsing failed.\n");
            free(input);
            continue;
        }

        execute_node(ast_root);
    }

    return 0;
}