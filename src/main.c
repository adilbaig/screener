/**
 * This file reads data into an mmap. Another process then reads that data and echoes it out
 */

#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "../headers/commands.h"

/**
 * Need to support :
 *
 * - networking : Allow connecting to the server and issuing commands
 * - sort : Sort table by column. Sort results of a find/filter
 * - skip/limit : on results
 * - filter : By value, by various conditions
 * - medians : On table, or on results of a find
 * - averages : as above
 * - find - Find by comparison of values or functions
 * - add user defined functions
 * - return a subset of columns from a query
 * - build/teardown indexes on the fly.
 */

int main(int argc, char *argv[]) {

    //Read strings from a file and load them in memory
    //Access and read them from a given position

    struct Table table;
    load_table(&table, argv[1]);

    //table_print(&table);

    while (1) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        printf("COMMAND : ");

        if ((read = getline(&line, &len, stdin)) > 1) {
            line[read - 1] = '\0';
        } else {
            continue;
        }

        execute_command(line, &table);

        if (read > 0)
            free(line);
    }

    table_free(&table);

    exit(EXIT_SUCCESS);
}

void load_table(struct Table* table, char* file) {
    FILE *stream = fopen(file, "r");
    if (stream == NULL) {
        perror("Could not open file");
        exit(EXIT_FAILURE);
    }

    table_init(table);

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stream)) != -1) {
        struct Vector* vector = malloc(sizeof(struct Vector));
        vector_init(vector);
        vectorize_csv(vector, line, read); // we don't pass the new line char
        //vector_print(&vector);
        table_append(table, vector);
    }

    free(line);
    fclose(stream);
}

void execute_command(char* cmd, struct Table* table) {
    //strdup mallocs and returns a pointer. Free it!
    char* cmd_copy = strdup(cmd);
    char* tok = strtok(cmd_copy, " ");

    if (strcmp(cmd, "EXIT") == 0) {
        exit(0);
    }else if (strcmp(cmd, "PRINT COLUMNS") == 0) {
        print_columns(table);
    } else if (strcmp(tok, "IS_COLUMN") == 0) {
        char* search = strtok(NULL, " ");

        printf("IS_COLUMN '%s'? : ", search);
        int pos;
        if (search != NULL && (pos = is_column(table, search)) > -1) {
            printf("TRUE (%i)\n", pos);
        } else {
            printf("FALSE\n");
        }
    } else if (strcmp(tok, "COLUMN") == 0) {
        char* search = strtok(NULL, " ");

        printf("Searching for COLUMN '%s'\n", search);
        if (search != NULL) {
            print_column(table, search);
        }
    } else {
        printf("PRINT COLUMNS, IS_COLUMN <COLUMN NAME>, COLUMN <COLUMN NAME>, SORT_BY <COLUMN_NAME>, FILTER <COLUMN NAME>, EXIT\n");
    }

    free(cmd_copy);
}

void vectorize_csv(struct Vector *vector, char *str, size_t length) {
    int l = 0;
    int last_comma = 0;

    //Go through *str char by char ..
    for (int i = 0; i < length; i++) {
        // .. when you hit one of [,\n] or EOL, process the remaining string into a vector entry
        if (str[i] == ',' || i + 1 == length) {
            l = i - last_comma;
            //printf("%i) Address %x, '%s', Length = %i \n", i, &str[last_comma], &str[last_comma], l);
            vector_append(vector, &str[last_comma], l);
            last_comma = i + 1;
        }
    }
}
