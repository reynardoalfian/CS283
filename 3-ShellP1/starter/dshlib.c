#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
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


int build_cmd_list(char *cmd_line, command_list_t *clist) {
    // Validate input
    if (cmd_line == NULL || clist == NULL) {
        return WARN_NO_CMDS;
    }
    // Initialize command list
    memset(clist, 0, sizeof(command_list_t));


    char *trimmed_cmd = trim(cmd_line);
    if (strlen(trimmed_cmd) == 0) {
        return WARN_NO_CMDS; 
    }

    // Create a copy of the command line for tokenization
    char cmd_copy[ARG_MAX];
    strncpy(cmd_copy, trimmed_cmd, ARG_MAX - 1);
    cmd_copy[ARG_MAX - 1] = '\0';

    char *saveptr;
    char *cmd_token = strtok_r(cmd_copy, PIPE_STRING, &saveptr);
    while (cmd_token != NULL) {
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        // Process individual command
        char *trimmed_token = trim(cmd_token);
        char *space_pos = strchr(trimmed_token, SPACE_CHAR);

        if (space_pos == NULL) {

            if (strlen(trimmed_token) >= EXE_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strncpy(clist->commands[clist->num].exe, trimmed_token, EXE_MAX - 1);
            clist->commands[clist->num].exe[EXE_MAX - 1] = '\0';
            clist->commands[clist->num].args[0] = '\0';
        } else {

            *space_pos = '\0';
            char *cmd_name = trim(trimmed_token);
            char *args = trim(space_pos + 1);

            if (strlen(cmd_name) >= EXE_MAX || strlen(args) >= ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }

            strncpy(clist->commands[clist->num].exe, cmd_name, EXE_MAX - 1);
            clist->commands[clist->num].exe[EXE_MAX - 1] = '\0';
            strncpy(clist->commands[clist->num].args, args, ARG_MAX - 1);
            clist->commands[clist->num].args[ARG_MAX - 1] = '\0';
        }

        clist->num++;
        cmd_token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    return OK;
}