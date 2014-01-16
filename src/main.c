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
#include "../headers/network.h"

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

struct Table table;

int main(int argc, char *argv[]) {

    //Read strings from a file and load them in memory
    //Access and read them from a given position

    if(argc < 3) {
        fprintf(stderr, "Usage: %s HOST PORT CSV_FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    load_table(&table, argv[3]);

    printf("Table loaded\n");
    //table_print(&table);

    unsigned short port = (unsigned short)atoi(argv[2]);
    int sfd = screener_listen(argv[1], port);
    printf("Listening on %s:%d\n", argv[1], port);

    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    while (1) {
        int accept_fd = accept(sfd, &addr, &addr_len);
        if(accept_fd < 0) {
            perror("Accept error");
            return 1;
        }

        //Fork a child to handle the socket
        int pid = fork();
        if (pid < 0) {
            perror("ERROR on fork");
            exit(1);
        }

        /* This is the new process */
        if (pid == 0) {
            close(sfd);
            run_child(accept_fd);
            exit(0);
        } else {
            //This is the parent process
            close(accept_fd);
        }
    }

    if(close(sfd) < 0) {
        perror("close socket error");
        return 1;
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

void execute_command(char* cmd, struct Table* table, FILE* fd_out) {
    //strdup mallocs and returns a pointer. Free it!
    char* cmd_copy = strdup(cmd);
    char* tok = strtok(cmd_copy, " ");
    char* response;

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

        printf("Searching for COLUMN %s \n", search);
        if (search) {
            printf("NOW IN PRINT_COM");
            print_column(table, search, fd_out);
        }
    } else {
        response = "PRINT COLUMNS, IS_COLUMN <COLUMN NAME>, COLUMN <COLUMN NAME>, SORT_BY <COLUMN_NAME>, FILTER <COLUMN NAME>, EXIT\n";
        fprintf(fd_out, response);
        printf("RESPONSE : %s", response);
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

void run_child(int accept_fd)
{
    /* Create child process */
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Don't read line by line. Read everything and tokenize
    FILE* file_fd = fdopen (accept_fd, "r+");
    if ((read = getline(&line, &len, file_fd)) > 1) {
        line[--read] = '\0';

        /**
         * Open a streaming string buffer
         */
        char *bp;
        size_t bp_size;
        FILE* stream = open_memstream (&bp, &bp_size);

        printf("COMMAND : [\"%s\"]\n", line);
        execute_command(line, &table, stream);

        fflush(stream);
        write(accept_fd, bp, bp_size);
        fclose(stream);

        free(line);
    }

    printf("Connection closed\n");
    fclose(file_fd);
    close(accept_fd);
}
