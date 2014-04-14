/*
 * main.c
 * This programs reads instruments from instruments.csv and creates an internal tree
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include "../headers/vector.h"

struct Vector rics;

int usage(char *program)
{
	printf("Usage : %s CSV\n", program);
	return 1;
}

int main(int argc, char *argv[])
{
	// Read a file of company TRBC codes and RICs
	if(argc < 2) {
		return usage(argv[0]);
	}

	FILE *stream = fopen(argv[1], "r");
	if (!stream) {
		perror("Could not open file");
		exit(1);
	}

	vector_init(&rics);

	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	while((read = getline(&line, &len, stream)) > 0 )
	{
		char code[10];
		char *ric = NULL;

		printf("%s\n", line);

		int scret = sscanf(line, "\"%10s\",\"%m[a-zA-Z\.]\"", code, &ric);
		if(scret > 0) {
			printf("Code : %s, RIC : %s\n", code, ric);


			/* Why assign to NULL and check?
			 * Because, if sscanf fails on a line, ric will not be assigned
			 * but we will attempt to free it. To avoid that, we check if
			 * ric has been assigned by checking if the address it is pointing
			 * to has changed.
			 */
			if(ric != NULL) {
				int idx = vector_append(&rics, ric);//, strlen(ric));
				//add_node(code, idx);
				// Now add it to the tree
				free(ric);
			}
		}
	}

	if(line != NULL) {
		free(line);
	}

	getc(stdin);

	vector_print(&rics);
	vector_free(&rics);

	fclose(stream);
	return 0;
}
