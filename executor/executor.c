#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "../AST/ast.h"
#include "../utils/utils.h"
#include "executor.h"

int execute_command(AstNode *node){
    if(node->type != NODE_COMMAND) return -1;

    if(node->command.argv && node->command.argv[0]){
        if(strcmp(node->command.argv[0], "exit") == 0){
            exit(0);
        }
        else if(strcmp(node->command.argv[0], "cd") == 0){
            const char *dir = node->command.argv[1] ? node->command.argv[1] : getenv("HOME");
            if(chdir(dir) != 0)
                perror("cd failed");
            return 0;
        }
    }

    Assignment *curr_assignment = node->command.assignments;
    while(curr_assignment){
        setenv(curr_assignment->var_name, curr_assignment->value, 1);
        curr_assignment = curr_assignment->next;
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("fork failed");
        return -1;
    }
    else if(pid == 0){
        Redirection *redir = node->command.redirections;
        while(redir){
            int fd;
            if(redir->redir_type == OUT)
                fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            else if(redir->redir_type == APPEND)
                fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0777);
            else if(redir->redir_type == IN)
                fd = open(redir->target, O_RDONLY);
            else{
                fprintf(stderr, "Unknown redirection type\n");
                exit(EXIT_FAILURE);
            }

            if(fd < 0){
                perror("open redirection failed");
                exit(EXIT_FAILURE);
            }

            if(redir->redir_type == IN)
                dup2(fd, STDIN_FILENO);
            else
                dup2(fd, STDOUT_FILENO);

            close(fd);
            redir = redir->next;
        }

        if(node->command.argv[0]){
            execvp(node->command.argv[0], node->command.argv);
            perror("hy345sh: ");
        }

        exit(EXIT_FAILURE);
    }
    else{
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

int execute_pipeline(AstNode *node){
    if(node->type != NODE_PIPELINE) return -1;

    if(node->pipeline.count == 1) return execute_command(node->pipeline.commands[0]);

    int num_cmds = node->pipeline.count;
    int pipefds[2*num_cmds];
    pid_t pids[num_cmds];

    for(int i=0;i<num_cmds-1;i++)
        if(pipe(pipefds + i*2) < 0) { perror("pipe failed"); exit(EXIT_FAILURE); }

    for(int i=0;i<num_cmds;i++){
        pids[i] = fork();
        if(pids[i] < 0) { perror("fork failed"); exit(EXIT_FAILURE); }
        else if(pids[i] == 0){
            /* Child process */
            if(i != 0) // Not first command
                dup2(pipefds[(i-1)*2], STDIN_FILENO);
            if(i != num_cmds-1) // Not last command
                dup2(pipefds[i*2+1], STDOUT_FILENO);

            for(int j=0;j<2*(num_cmds-1);j++)
                close(pipefds[j]);

            execute_node(node->pipeline.commands[i]);
            exit(EXIT_FAILURE);
        }
    }

    for(int i=0;i<2*(num_cmds-1);i++) close(pipefds[i]);
    int status;
    for(int i=0;i<num_cmds;i++) waitpid(pids[i], &status, 0);

    return WEXITSTATUS(status);
}

int execute_sequence(AstNode *node){
    if(node->type != NODE_SEQUENCE) return -1;
    int status = 0;

    status = execute_node(node->sequence.left);
    if(node->sequence.right)
        status = execute_node(node->sequence.right);

    return status;
}

int execute_if(AstNode *node){
    if(node->type != NODE_IF) return -1;
    int status = execute_node(node->if_stmt.condition);
    if(status == 0) // success
        return execute_node(node->if_stmt.then_branch);
    return 0;
}

int execute_for(AstNode *node){
    if(node->type != NODE_FOR) return -1;

    for(int i=0; node->forloop.items[i]!=NULL && strlen(node->forloop.items[i])>0; i++){
        setenv(node->forloop.var_name, node->forloop.items[i], 1);
        execute_node(node->forloop.body);
    }
    return 0;
}

int execute_node(AstNode *node){
    if(!node) return -1;

    switch(node->type){
        case NODE_COMMAND:
            return execute_command(node);
        case NODE_PIPELINE:
            return execute_pipeline(node);
        case NODE_SEQUENCE:
            return execute_sequence(node);
        case NODE_IF:
            return execute_if(node);
        case NODE_FOR:
            return execute_for(node);
        default:
            fprintf(stderr, "Unknown AST node type %d\n", node->type);
            return -1;
    }
}