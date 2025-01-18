#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
int  setup_buff(char *buff, const char *user_str, int len);
//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_string(char *buff, int str_len);
int  string_replace(char *buff, int, const char *, const char *);
int  word_print(char *buff, int str_len);

int setup_buff(char *buff, const char *user_str, int len) {
    if (!buff || !user_str) {
        return -2;
    }

    char *dest = buff;
    const char *src = user_str;

    int count = 0;
    int in_space = 0;

    while (*src != '\0') {
        int is_whitespace = 0;
        if (*src == ' ' || *src == '\t' || *src == '\n') {
            is_whitespace = 1;
        }

        if (is_whitespace) {

            if (!in_space && count > 0 && count < len) {
                *dest = ' ';
                dest++;
                count++;
                in_space = 1;
            }
        } else {

            if (count < len) {
                *dest = *src;
                dest++;
                count++;
            } else {

                return -1;
            }
            in_space = 0;
        }
        src++;
    }


    if (count > 0 && buff[count - 1] == ' ') {
        count--;
        dest--;
        *dest = '.';
    }


    while (count < len) {
        *dest = '.';
        dest++;
        count++;
    }

    int processed_len = 0;
    for (int i = 0; i < len; i++) {
        if (buff[i] == '.') {
            break;
        }
        processed_len++;
    }

    return processed_len;
}



void print_buff(char *buff) {
    printf("Buffer:  ");
    for (int i = 0; i < BUFFER_SZ; i++) {
        putchar(buff[i]);
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    if (!buff || str_len <= 0) {
            return 0;
        }

        int word_count = 0;
        int in_word    = 0;
        char *p = buff;
        for (int i = 0; i < str_len; i++, p++) {
            if (*p != ' ') {
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

void reverse_string(char *buff, int str_len) {
    if (!buff || str_len <= 1) {
        return;
    }
    char *start = buff;
    char *end   = buff + (str_len - 1);

    while (start < end) {
        char temp = *start;
        *start    = *end;
        *end      = temp;

        start++;
        end--;
    }
}

int string_replace(char *buff, int buff_size, const char *search, const char *replace) {
    if (!buff || !search || !replace) {
        return -1;
    }

    int search_len = 0;
    const char *s = search;
    while (*s != '\0') {
        search_len++;
        s++;
    }

    int replace_len = 0;
    const char *r = replace;
    while (*r != '\0') {
        replace_len++;
        r++;
    }

    char *found = NULL;
    int i = 0;
    while (i <= buff_size - search_len) {
        int match = 1;
        for (int j = 0; j < search_len; j++) {
            if (buff[i + j] != search[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            found = buff + i;
            break;
        }
        i++;
    }

    if (found == NULL) {
        return 0;
    }

    int current_len = 0;
    while (current_len < buff_size && buff[current_len] != '.') {
        current_len++;
    }


    int new_length = current_len - search_len + replace_len;
    if (new_length > buff_size) {
        return -1;
    }


    if (replace_len > search_len) {
        int shift = replace_len - search_len;
        for (int k = current_len; k >= (found - buff) + search_len; k--) {
            if (k + shift < buff_size) {
                buff[k + shift] = buff[k];
            }
        }
    } else if (replace_len < search_len) {
        int shift = search_len - replace_len;

        for (int k = (found - buff) + search_len; k < current_len; k++) {
            buff[k - shift] = buff[k];
        }
 
        for (int k = new_length; k < buff_size; k++) {
            buff[k] = '.';
        }
    }

    for (int j = 0; j < replace_len; j++) {
        found[j] = replace[j];
    }


    if (replace_len >= search_len) {
        for (int k = new_length; k < buff_size; k++) {
            buff[k] = '.';
        }
    }

    return 1;
}


int word_print(char *buff, int str_len) {
    if (!buff || str_len <= 0) {
        return -1;
    }

    printf("Word Print\n");
    printf("----------\n");

    char *p       = buff;
    int count     = 0;
    int in_word   = 0; 
    char *w_start = NULL;

    int i;
    for (i = 0; i < str_len; i++) {
        if (p[i] != ' ') {
            if (!in_word) {
                in_word   = 1;
                w_start   = &p[i];
                count++;
            }
        } else {

            if (in_word) {
                in_word = 0;
                int length = (&p[i]) - w_start;
                printf("%d. ", count);
                for (char *q = w_start; q < &p[i]; q++) {
                    putchar(*q);
                }
                printf("(%d)\n", length);
            }
        }
    }


    if (in_word) {
        int length = (&p[i]) - w_start;
        printf("%d. ", count);
        for (char *q = w_start; q < &p[i]; q++) {
            putchar(*q);
        }
        printf("(%d)\n", length);
    }

    return count;
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

    opt = *(argv[1]+1);   //get the option flag

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

    for(int i = 0; i < BUFFER_SZ; i++) {
        buff[i] = '.';
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0){

        if (user_str_len == -1) {
            fprintf(stderr, "error: Provided input string is too long\n");
            free(buff);
            exit(3);
        } else {
            fprintf(stderr, "error: Unknown error in setup_buff()\n");
            free(buff);
            exit(3);
        }
    }

    switch (opt){
        case 'c': {
            // Count words
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0) {
                fprintf(stderr, "Error counting words, rc = %d\n", rc);
                free(buff);
                exit(3);
            }
            printf("Word Count: %d\n", rc);
            printf("Buffer:  [");
            for (int i = 0; i < BUFFER_SZ; i++) {
                putchar(buff[i]);
            }
            printf("]\n");
            break;
        }

        case 'r': {
            // Reverse
            reverse_string(buff, user_str_len);
            printf("Buffer:  [");
            for (int i = 0; i < BUFFER_SZ; i++) {
                putchar(buff[i]);
            }
            printf("]\n");
            break;
        }

        case 'w': {
            // Print words
            rc = word_print(buff, user_str_len);
            if (rc < 0) {
                fprintf(stderr, "Error printing words, rc = %d\n", rc);
                free(buff);
                exit(3);
            }
            printf("\nNumber of words returned: %d\n", rc);
            printf("Buffer:  [");
            for (int i = 0; i < BUFFER_SZ; i++) {
                putchar(buff[i]);
            }
            printf("]\n");
            break;
        }

        case 'x': {
            // search/replace functions
            if (argc < 5) {
                fprintf(stderr, "Error: Missing search or replace string for -x option.\n");
                usage(argv[0]);
                free(buff);
                exit(1);
            }
            char *search  = argv[3];
            char *replace = argv[4];

            rc = string_replace(buff, BUFFER_SZ, search, replace);

            if (rc < 0 || rc == 0) {

                printf("Not Implemented!\n");
                free(buff);
                exit(3);
            }

            printf("Buffer:  [");
            for (int i = 0; i < BUFFER_SZ; i++) {
                putchar(buff[i]);
            }
            printf("]\n");
            break;
        }

        default: {
                    usage(argv[0]);
                    free(buff);
                    exit(1);
        }

    }
    //TODO:  #6 Dont forget to free your buffer before exiting
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