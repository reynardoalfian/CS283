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
    
    memset(cmd_buff->argv, 0, sizeof(char *) * CMD_ARGV_MAX);
    
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);

    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd) {
    
    if (!cmd_line || !cmd) return ERR_MEMORY;
    
    cmd->argc = 0;
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


int exec_local_cmd_loop() {
    cmd_buff_t cmd;
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
        
        // Parse the command
        rc = build_cmd_buff(cmd._cmd_buffer, &cmd);
        
        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }
        if (rc != OK) {
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
                execvp(cmd.argv[0], cmd.argv);
                perror("execvp");
                exit(ERR_EXEC_CMD);
            } 
            else {
                // Parent process
                int status;
                waitpid(pid, &status, 0);
                
                if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                    printf("Command failed with status %d\n", WEXITSTATUS(status));
                }
            }
        }
        
        clear_cmd_buff(&cmd);
    }
    
    free_cmd_buff(&cmd);
    return OK;
}