#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
int  string_replace(char *buff, int, const char *, const char *);

int setup_buff(char *buff, char *user_str, int len) {
    if (user_str == NULL) {
        return -1;
    }
    int user_str_len = snprintf(buff, len, "%s", user_str); 
    // Check if the user string was truncated
    if (user_str_len >= len) {
        return -1;
    }
    
    return user_str_len;
}

void print_buff(char *buff) {
    printf("Buffer:  %s\n", buff);
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int word_count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        if (buff[i] != ' ' && buff[i] != '\t' && buff[i] != '\n' && buff[i] != '\0') {
            if (!in_word) {
                in_word = 1;
                word_count++;
            }
        } else {
            in_word = 0;
        }
    }
    return word_count;
}

int string_replace(char *buff, int buff_size, const char *search, const char *replace) {
    char *pos = strstr(buff, search);
    if (!pos) {
        return 0; // No match found
    }

    int search_len = strlen(search);
    int replace_len = strlen(replace);
    int remaining_len = strlen(pos + search_len);

    int new_length = (pos - buff) + replace_len + remaining_len;
    if (new_length >= buff_size) {
        return -1;

    if (replace_len != search_len) {
        memmove(pos + replace_len, pos + search_len, remaining_len + 1);
    }

    memcpy(pos, replace, replace_len);
    return 1;
}
//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    // Ensure the program is invoked with sufficient arguments to avoid undefined behavior.
    // It checks for argc < 2 first to ensure that there is at least one argument after the program name
    // If argc < 2, argc[1] does not exist, attempting to access it would cause a crash
    // Checking argc[1] != '-' ensures that the first argument is a flag
    // If either condition fails, the program can exit safely aftert showing the usage message
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    // The check ensures that the program provided with at least two arguments after the executable name
    // A valid option flag
    // A user providde string for processing
    // Without these, the program cannot proceed and will exit safely with a usage message
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    buff = (char *)malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for buffer.\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            // Count words in the buffer
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        case 'r':
            // Reverse the string in the buffer
            for (int i = 0; i < user_str_len / 2; i++) {
                char temp = buff[i];
                buff[i] = buff[user_str_len - i - 1];
                buff[user_str_len - i - 1] = temp;
            }
            printf("Reversed String: %s\n", buff);
            break;

        case 'w':
            // Count the number of whitespace characters in the buffer
            rc = 0;
            for (int i = 0; i < user_str_len; i++) {
                if (buff[i] == ' ' || buff[i] == '\t' || buff[i] == '\n') {
                    rc++;
                }
            }
            printf("Whitespace Count: %d\n", rc);
            break;

        case 'x': {
            // Makes sure there are enough arguments for search and replace
            if (argc < 5) {
                fprintf(stderr, "Error: Missing search or replace string for -x option.\n");
                usage(argv[0]);
                exit(1);
            }

            char *search = argv[3];   // The string to search for
            char *replace = argv[4]; // The string to replace it with

            rc = string_replace(buff, BUFFER_SZ, search, replace);
            if (rc < 0) {
                fprintf(stderr, "Error: Buffer overflow during string replacement.\n");
                exit(2);
            }

            printf("Modified String: %s\n", buff);
            break;
        }
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          Providing buffer pointer and length in the helper functions improves flexibility, reusability, and safety.
//          It allows the functions to be used with any buffer of any size, not just the one in main().
//          The functions are also more generic and independent of specific buffer implementations. This way the function
//          can operate more reliably in different scenarios, ensuring memory is accessed correctly and safely.