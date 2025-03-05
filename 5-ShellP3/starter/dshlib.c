#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
char* trim(char* str) {
    if (str == NULL) {
        return NULL;
    }
    
    // Trim leading spaces
    char* start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }
    
    if (*start == '\0') {
        return start;
    }
    
    // Trim trailing spaces
    char* end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
    
        end--;
    }
    

    *(end + 1) = '\0';
    
    return start;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff == NULL) {
        return ERR_MEMORY;
    }

    // Allocate memory for command buffer
    cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }

    cmd_buff->argc = 0;
    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    
    memset(cmd_buff->argv, 0, sizeof(char *) * CMD_ARGV_MAX);
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);

    return OK;
}
int free_cmd_buff(cmd_buff_t *cmd_buff) {
    
    if (cmd_buff == NULL) {
        return ERR_MEMORY;
    }

    // Free the allocated buffer if it exists
    if (cmd_buff->_cmd_buffer != NULL) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }


    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(char *) * CMD_ARGV_MAX);

    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff == NULL || cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }

    // Reset command buffer to empty state
    cmd_buff->argc = 0;
    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    
    memset(cmd_buff->argv, 0, sizeof(char *) * CMD_ARGV_MAX);
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);

    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd) {
    if (!cmd_line || !cmd) return ERR_MEMORY;
    
    cmd->argc = 0;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    memset(cmd->argv, 0, sizeof(char *) * CMD_ARGV_MAX);
    
    char *pos = cmd_line;
    bool in_quotes = false;
    bool in_token = false;
    char *token_start = NULL;
    
    // Skip leading spaces
    while (*pos && isspace(*pos)) pos++;
    
    while (*pos) {
        if (*pos == '"') {
            if (!in_quotes) {
                in_quotes = true;
                if (!in_token) {
                    token_start = pos + 1;
                    in_token = true;
                }
            } else {
                in_quotes = false;
                *pos = '\0';
                if (cmd->argc >= CMD_ARGV_MAX - 1) {
                    return ERR_TOO_MANY_COMMANDS;
                }
                cmd->argv[cmd->argc++] = token_start;
                in_token = false;
            }
        } else if ((*pos == '<' || *pos == '>') && !in_quotes) {
            // Handle redirection operators
            if (in_token) {
                *pos = '\0';
                if (cmd->argc >= CMD_ARGV_MAX - 1) {
                    return ERR_TOO_MANY_COMMANDS;
                }
                cmd->argv[cmd->argc++] = token_start;
                in_token = false;
            }
            
            // Save the redirection operator
            char redirect_op = *pos;
            *pos = '\0';
            pos++;
            
            // Skip spaces after the redirection operator
            while (*pos && isspace(*pos)) pos++;
            
            // Get the filename
            if (*pos) {
                token_start = pos;
                
                // Find the end of the filename
                while (*pos && !isspace(*pos) && *pos != '<' && *pos != '>') pos++;
                
                // Null-terminate the filename
                if (*pos) {
                    *pos = '\0';
                    pos++;
                }
                
                // Store the filename
                if (redirect_op == '<') {
                    cmd->input_file = token_start;
                } else {
                    cmd->output_file = token_start;
                }
            }
            
            continue;
        } else if (isspace(*pos) && !in_quotes) {
            if (in_token) {
                *pos = '\0';
                if (cmd->argc >= CMD_ARGV_MAX - 1) {
                    return ERR_TOO_MANY_COMMANDS;
                }
                cmd->argv[cmd->argc++] = token_start;
                in_token = false;
            }
        } else if (!in_token) {
            token_start = pos;
            in_token = true;
        }
        pos++;
    }
    
    // Handle last token if it exists
    if (in_token) {
        if (cmd->argc >= CMD_ARGV_MAX - 1) {
            return ERR_TOO_MANY_COMMANDS;
        }
        cmd->argv[cmd->argc++] = token_start;
    }
    
    // NULL terminate argv array
    cmd->argv[cmd->argc] = NULL;
    
    return cmd->argc > 0 ? OK : WARN_NO_CMDS;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *token, *saveptr;
    int cmd_count = 0;
    char *cmd_copy = strdup(cmd_line); // Make a copy to avoid modifying original
    
    if (!cmd_copy) {
        return ERR_MEMORY;
    }
    
    // Initialize the command list
    clist->num = 0;
    
    // Tokenize by pipe character
    token = strtok_r(cmd_copy, PIPE_STRING, &saveptr);
    while (token != NULL && cmd_count < CMD_MAX) {
        // Trim spaces
        token = trim(token);
        
        // Parse the command and arguments
        char *space_pos = strchr(token, SPACE_CHAR);
        if (space_pos) {
            // Has arguments
            int exe_len = space_pos - token;
            if (exe_len >= EXE_MAX) {
                free(cmd_copy);
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            
            strncpy(clist->commands[cmd_count].exe, token, exe_len);
            clist->commands[cmd_count].exe[exe_len] = '\0';
            
            strncpy(clist->commands[cmd_count].args, token, ARG_MAX - 1);
            clist->commands[cmd_count].args[ARG_MAX - 1] = '\0';
        } else {
            // Command only, no arguments
            if (strlen(token) >= EXE_MAX) {
                free(cmd_copy);
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            
            strcpy(clist->commands[cmd_count].exe, token);
            strcpy(clist->commands[cmd_count].args, token);
        }
        
        cmd_count++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }
    
    free(cmd_copy);
    clist->num = cmd_count;
    
    if (cmd_count == 0) {
        return WARN_NO_CMDS;
    }
    
    if (cmd_count > CMD_MAX) {
        return ERR_TOO_MANY_COMMANDS;
    }
    
    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int i;
    int pipes[2 * (clist->num - 1)]; // Two file descriptors for each pipe
    pid_t pids[clist->num];
    
    // Create all the pipes needed
    for (i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes + i * 2) < 0) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }
    
    // Create and set up all processes
    for (i = 0; i < clist->num; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("fork");
            return ERR_EXEC_CMD;
        }
        
        if (pids[i] == 0) {
            // Child process
            
            // Execute the command
            cmd_buff_t cmd;
            if (alloc_cmd_buff(&cmd) != OK) {
                exit(ERR_MEMORY);
            }
            
            // Copy the command string into cmd buffer
            strcpy(cmd._cmd_buffer, clist->commands[i].args);
            
            // Parse the command
            if (build_cmd_buff(cmd._cmd_buffer, &cmd) != OK) {
                exit(ERR_EXEC_CMD);
            }
            
            // Connect pipes: set up stdin from previous command's pipe
            if (i > 0) {
                if (dup2(pipes[(i-1)*2], STDIN_FILENO) < 0) {
                    perror("dup2");
                    exit(ERR_EXEC_CMD);
                }
            } else if (cmd.input_file) {
                // Input redirection for first command in pipeline
                int fd = open(cmd.input_file, O_RDONLY);
                if (fd < 0) {
                    perror("open input");
                    exit(ERR_EXEC_CMD);
                }
                if (dup2(fd, STDIN_FILENO) < 0) {
                    perror("dup2 input");
                    exit(ERR_EXEC_CMD);
                }
                close(fd);
            }
            
            // Connect pipes: set up stdout to next command's pipe
            if (i < clist->num - 1) {
                if (dup2(pipes[i*2 + 1], STDOUT_FILENO) < 0) {
                    perror("dup2");
                    exit(ERR_EXEC_CMD);
                }
            } else if (cmd.output_file) {
                // Output redirection for last command in pipeline
                int fd = open(cmd.output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("open output");
                    exit(ERR_EXEC_CMD);
                }
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("dup2 output");
                    exit(ERR_EXEC_CMD);
                }
                close(fd);
            }
            
            // Close all pipe file descriptors
            for (int j = 0; j < 2 * (clist->num - 1); j++) {
                close(pipes[j]);
            }
            
            // Execute
            execvp(cmd.argv[0], cmd.argv);
            perror("execvp");
            exit(ERR_EXEC_CMD);
        }
    }
    
    // Parent process: close all pipe fds
    for (i = 0; i < 2 * (clist->num - 1); i++) {
        close(pipes[i]);
    }
    
    // Wait for all children
    for (i = 0; i < clist->num; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            printf("Command %d failed with status %d\n", i, WEXITSTATUS(status));
        }
    }
    
    return OK;
}

int exec_local_cmd_loop() {
    cmd_buff_t cmd;
    command_list_t clist;
    int rc;
    
    rc = alloc_cmd_buff(&cmd);
    if (rc != OK) {
        return ERR_MEMORY;
    }
    
    while (1) {
        printf("%s", SH_PROMPT);
        
        if (fgets(cmd._cmd_buffer, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        // Remove trailing newline
        cmd._cmd_buffer[strcspn(cmd._cmd_buffer, "\n")] = '\0';
        
        // Check if the command contains pipes
        bool has_pipes = strchr(cmd._cmd_buffer, PIPE_CHAR) != NULL;
        
        // REMOVE ALL CODE BETWEEN HERE AND THE if (has_pipes) BLOCK
        
        if (has_pipes) {
            // Parse the command into a command list
            rc = build_cmd_list(cmd._cmd_buffer, &clist);
            
            if (rc == WARN_NO_CMDS) {
                printf(CMD_WARN_NO_CMD);
                continue;
            } else if (rc == ERR_TOO_MANY_COMMANDS) {
                printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                continue;
            } else if (rc != OK) {
                printf("Error parsing command\n");
                continue;
            }
            
            // Execute the pipeline
            execute_pipeline(&clist);
        } else {
            // No pipes, handle as a single command
            rc = build_cmd_buff(cmd._cmd_buffer, &cmd);
            
            if (rc == WARN_NO_CMDS) {
                printf(CMD_WARN_NO_CMD);
                continue;
            } else if (rc != OK) {
                printf("Error parsing command\n");
                continue;
            }
            
            // Handle built-in commands
            if (cmd.argc > 0) {
                if (strcmp(cmd.argv[0], EXIT_CMD) == 0) {
                    break;
                }
                if (strcmp(cmd.argv[0], "dragon") == 0) {
                    print_dragon();
                    continue;
                }
                if (strcmp(cmd.argv[0], "cd") == 0) {
                    if (cmd.argc > 1) {
                        if (chdir(cmd.argv[1]) != 0) {
                            perror("cd");
                        }
                    }
                    continue;
                }
                
                // Handle external command
                pid_t pid = fork();
                
                if (pid < 0) {
                    perror("fork");
                    continue;
                }
                
                if (pid == 0) {
                    // Child process
                    
                    // Handle input redirection
                    if (cmd.input_file) {
                        int fd = open(cmd.input_file, O_RDONLY);
                        if (fd < 0) {
                            perror("open input");
                            exit(ERR_EXEC_CMD);
                        }
                        if (dup2(fd, STDIN_FILENO) < 0) {
                            perror("dup2 input");
                            exit(ERR_EXEC_CMD);
                        }
                        close(fd);
                    }
                    
                    // Handle output redirection
                    if (cmd.output_file) {
                        int fd = open(cmd.output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd < 0) {
                            perror("open output");
                            exit(ERR_EXEC_CMD);
                        }
                        if (dup2(fd, STDOUT_FILENO) < 0) {
                            perror("dup2 output");
                            exit(ERR_EXEC_CMD);
                        }
                        close(fd);
                    }
                    
                    execvp(cmd.argv[0], cmd.argv);
                    perror("execvp");
                    exit(ERR_EXEC_CMD);
                } else {
                    // Parent process
                    int status;
                    waitpid(pid, &status, 0);
                    
                    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                        printf("Command failed with status %d\n", WEXITSTATUS(status));
                    }
                }
            }
        }
        
        clear_cmd_buff(&cmd);
    }
    
    free_cmd_buff(&cmd);
    return OK;
}