#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CLEAR_SCREEN    "\e[1;1H\e[2J"
#define RED             "\x1b[31m"
#define GREEN           "\x1b[32m"
#define YELLOW          "\x1b[33m"
#define BLUE            "\x1b[34m"
#define MAGENTA         "\x1b[35m"
#define CYAN            "\x1b[36m"
#define RESET_COLOUR    "\x1b[0m"

char** str_split(char* a_str, const char a_delim) {
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp) {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result) {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

void touch(char* filename, char* filecontents) {
    FILE* fp;
    
    fp = fopen(filename, "w+");
    if (filecontents != NULL) {
        fputs(filecontents, fp);
        fputc('\n', fp);
    }
    fclose(fp);
}

void cat(char* filename) {
    FILE* fp;

    fp = fopen(filename, "r");
    if (fp) {
        char c;
        c = fgetc(fp);

        while (c != EOF) {
            printf("%c", c);
            c = fgetc(fp);
        }

        fclose(fp);
    }
    else {
        printf("cat: %s: No such file\n", filename);
    }
}

void rm(char* filename) {
    FILE* fp;

    fp = fopen(filename, "r");
    if (fp) {
        fclose(fp);
        remove(filename);
    }
    else {
        printf("rm: cannot remove '%s': No such file", filename);
    }
}

void mv(char* input_filename, char* output_filename) {
    FILE* fp1;
    FILE* fp2;

    fp1 = fopen(input_filename, "r");
    
    if (fp1) {
        int a;
        
        fp2 = fopen(output_filename, "w");

        while ((a = fgetc(fp1)) != EOF) {
            fputc(a, fp2);
        }

        fclose(fp1);

        remove(input_filename);

        fclose(fp2);
    }
    else {
        printf("mv: cannot rename '%s': No such file", input_filename);
    }
}

void exit_terminal(FILE* file_list) {
    char line[512];
    fseek(file_list, 0, SEEK_SET);

    while (fgets(line, sizeof(line), file_list)) {
        line[strcspn(line, "\n")] = 0;
        remove(line);
    }

    fclose(file_list);

    remove("files");
}

int main() {
    char cline_input[1024];
    char** cline_arguments;
    
    FILE* list_of_files;
    list_of_files = fopen("files", "a+");

    printf(CLEAR_SCREEN);

    printf(MAGENTA "user" RESET_COLOUR "@" YELLOW "terminal" RESET_COLOUR ":" GREEN "~" RESET_COLOUR "$ ");
    fgets(cline_input,1024,stdin);
    cline_input[strcspn(cline_input, "\n")] = 0;

    cline_arguments = str_split(cline_input, ' ');

    while (strcmp(*(cline_arguments), "exit") != 0) {
        int i;

        for (i = 0; *(cline_arguments + i); i++) {
        }

        if (strcmp(*(cline_arguments), "touch") == 0) {

            if ((i < 2) != (i > 3)) {
                printf("touch: Usage: 'touch: <file name> <file contents>'\n");
            }
            else {
                touch(*(cline_arguments + 1), *(cline_arguments + 2));
                fprintf(list_of_files, "%s\n", *(cline_arguments + 1));
            }

        }

        else if (strcmp(*(cline_arguments), "cat") == 0) {

            if (i != 2) {
                printf("cat: Usage: 'cat: <file name>'\n");
            }
            else {
                cat(*(cline_arguments + 1));
            }

        }

        else if (strcmp(*(cline_arguments), "rm") == 0) {
            
            if (i != 2) {
                printf("rm: Usage: 'rm: <file name>'\n");
            }
            else {
                rm(*(cline_arguments + 1));
            }

        }

        else if (strcmp(*(cline_arguments), "mv") == 0) {

            if ((i < 2) != (i > 3)) {
                printf("mv: Usage: 'mv: <file name> <file contents>'\n");
            }
            else {
                mv(*(cline_arguments + 1), *(cline_arguments + 2));
                fprintf(list_of_files, "%s\n", *(cline_arguments + 2));
            }

        }

        else {
            printf("%s: command not found\n", *(cline_arguments));
        }


        if (cline_arguments) {
            for (int j = 0; *(cline_arguments + j); j++) {
                //printf("command=[%s]\n", *(cline_arguments + i));
                free(*(cline_arguments + j));
            }

            //printf("\n");
            free(cline_arguments);
        }

        printf(MAGENTA "user" RESET_COLOUR "@" YELLOW "terminal" RESET_COLOUR ":" GREEN "~" RESET_COLOUR "$ ");

        fgets(cline_input, 1024, stdin);

        cline_input[strcspn(cline_input, "\n")] = 0;

        cline_arguments = str_split(cline_input, ' ');
    }

    if (cline_arguments) {
        for (int j = 0; *(cline_arguments + j); j++) {
            free(*(cline_arguments + j));
        }

        free(cline_arguments);
    }

    exit_terminal(list_of_files);

    printf(CLEAR_SCREEN);

    return 0;
}
