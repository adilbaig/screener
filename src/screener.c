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
#include "vector.c"
#include "table.c"

/**
 * Need to support :
 *
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

	FILE *stream = fopen(argv[1], "r");
	if (stream == NULL) {
		perror("Could not open file");
		exit(EXIT_FAILURE);
	}

	struct Table table;
	table_init(&table);

	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	while ((read = getline(&line, &len, stream)) != -1) {
		struct Vector* vector = malloc(sizeof(struct Vector));
		vector_init(vector);
		vectorize_csv(vector, line, read); // we don't pass the new line char
		//vector_print(&vector);
		table_append(&table, vector);
	}

	free(line);
	fclose(stream);

	//table_print(&table);

	printf("Get RIC\n");
	char **rez;

	/**
	 * CAREFUL! Every time table_get_values > 0, it has malloced. Free it!
	 */
	if (table_get_values(&table, "RIC", &rez) >= 0) {
		for (int i = 0; i < table.length - 1; i++)
			printf("'%s', ", rez[i]);
		free(rez);
	}

	if (table_get_values(&table, "AssetCategory", &rez) >= 0) {
		for (int i = 0; i < table.length - 1; i++)
			printf("'%s', ", rez[i]);
		free(rez);
	}

	pause();

	table_free(&table);

	exit(EXIT_SUCCESS);
}

/*
 * FILE *fp;
           char *line = NULL;
           size_t len = 0;
           ssize_t read;

           fp = fopen("/etc/motd", "r");
           if (fp == NULL)
               exit(EXIT_FAILURE);

           while ((read = getline(&line, &len, fp)) != -1) {
               printf("Retrieved line of length %zu :\n", read);
               printf("%s", line);
           }

           free(line);
 */
void vectorize_csv(struct Vector *vector, char *str, size_t length) {
	int l = 0;
	int last_comma = 0;

	//Go through *str char by char ..
	for (int i = 0; i < length; i++) {
		// .. when you hit one of [,\n] or end, process the remaining string into a vector entry
		if (str[i] == ',' || i + 1 == length) {
			l = i - last_comma;
			//printf("%i) Address %x, '%s', Length = %i \n", i, &str[last_comma], &str[last_comma], l);
			vector_append(vector, &str[last_comma], l);
			last_comma = i + 1;
		}
	}
}
