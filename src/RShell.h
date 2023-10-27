#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <setjmp.h>
#include <direct.h>

#define BUF_SIZE 1024
#define TOK_BUF_SIZE 64
#define TOK_DELIMITER " \t\r\n\a"

static jmp_buf jump_buffer;

void check_buffer(char **buffer) {
    if(!(*buffer)) {
        printf("- Allocation error...\n");
        exit(EXIT_FAILURE);
    }
}

char *read_line(void) {
    int buffer_size = BUF_SIZE, position = 0, c;
    char *buffer = (char*) malloc(sizeof(char) * buffer_size);

    check_buffer(&buffer);

    for(;;) {
        c = getchar();

        if(c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;

        } else {
            buffer[position] = c;
        }

        position++;

        if(position >= buffer_size) {
            buffer_size += BUF_SIZE;
            buffer = (char*) realloc(buffer, buffer_size);

            check_buffer(&buffer);
        }
    }
}

char **split_line(char *line) {
    int buffer_size = TOK_BUF_SIZE, position = 0;
    char **tokens = (char**) malloc(sizeof(char*) * buffer_size);
    char *token;

    check_buffer(tokens);

    token = strtok(line, TOK_DELIMITER);
    while(token != NULL) {
        tokens[position] = token;
        position++;

        if(position >= buffer_size) {
            buffer_size += TOK_BUF_SIZE;
            tokens = (char**) realloc(tokens, buffer_size * sizeof(char*));

            check_buffer(tokens);
        }

        token = strtok(NULL, TOK_DELIMITER);
    }

    tokens[position] = NULL;
    return tokens;
}

bool execute(char **args, size_t argc) {
    if(argc <= 0) {
        printf("- The command has not been given");
        return false;
    }

    if(setjmp(jump_buffer) == 0) {
        if(strcmp(args[0], "cls") == 0) {
            system("cls");
            return true;

        } else if(strcmp(args[0], "echo") == 0) {
            for(int i = 1; i < argc; i++) {
                printf("%s ", args[i]);
            }

            printf("\n\n");
            return true;

        } else if(strcmp(args[0], "rf") == 0) {
            FILE* file = fopen(args[1], "r");
            
            if(file == NULL) {
                perror("- Error when trying to open the file");
                return false;
            }

            char line[1024];
            while(fgets(line, sizeof(line), file) != NULL) {
                printf("%s\n\n", line);
            }

            fclose(file);
            return true;
            
        } else if(strcmp(args[0], "wf") == 0) {
            FILE* file = fopen(args[1], "w");

            if(file == NULL) {
                perror("- Error trying to write to file");
                return false;
            }

            for(int i = 2; i < argc; i++) {
                fprintf(file, "%s", args[i]);

                if(i < (argc - 1)) {
                    fprintf(file, " ");
                }
            }

            fclose(file);
            return true;

        } else if(strcmp(args[0], "mkdir") == 0) {
            int result = _mkdir(args[1]);

            if(result != 0) {
                printf("- Error when trying to create %s directory", args[1]);
                exit(EXIT_FAILURE);
            }

            return true;
        }

        printf("- Invalid command.");
        return false;

    } else {
        printf("- Error when trying to execute the command.");
        return false;
    }
}

void loop(void) {
    bool status;

    do {
        printf("> ");

        char *line = read_line();
        char **args = split_line(line);

        size_t argc = 0;
        while(args[argc] != NULL) argc++;

        status = execute(args, argc);

        free(line);
        free(args);

    } while (status);
}