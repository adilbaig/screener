#include "../headers/vector.h"

/** ------------ PRIVATE ------------ **/

//static == (private in PHP)
static void vector_increase_capacity(struct Vector *vector, int capacity) {
	vector->capacity += capacity;
	vector->data = realloc(vector->data, sizeof(size_t) * vector->capacity);
}

/** ------------ PUBLIC ------------ **/

void vector_init(struct Vector *vector) {
	vector->length = 0;
	vector->capacity = 100;

	vector->data = malloc(sizeof(size_t) * vector->capacity);
}

/**
 * Duplicate the string "value" and append it to vector
 */
int vector_append_copy(struct Vector *vector, const char *value, size_t l) {
	// make sure there's room to expand into
	if (vector->length >= vector->capacity) {
		vector_increase_capacity(vector, 100);
	}

	char *_ptr = strndup(value, l);
	vector->data[vector->length++] = _ptr;
	return vector->length;
}

/**
 * Append the pointer "value" to vector
 *
 * returns the index
 */
int vector_append(struct Vector *vector, char *value) {
	// make sure there's room to expand into
	if (vector->length >= vector->capacity) {
		vector_increase_capacity(vector, 100);
	}

	vector->data[vector->length++] = value;
	return vector->length;
}

char* vector_get(struct Vector *vector, int index) {
	if (index >= vector->length || index < 0) {
		printf("Index %d out of bounds for vector of length %d\n", index, vector->length);
		exit(1);
	}

	return vector->data[index];
}

ssize_t vector_find(struct Vector *vector, const char value[]) {
	for (int i = 0; i < vector->length; i++) {
//		printf("i = %i, '%s' == '%s'. CMP = %i\n", i, vector->data[i], value, strcmp(vector->data[i], value));

		if (strcmp(vector->data[i], value) == 0)
			return i;
	}

	return -1;
}

void vector_print(struct Vector *vector) {
	printf("Printing %i values from row\n", vector->length);

	for (int i = 0; i < vector->length; i++) {
		printf("%i '%s'\n", i, vector_get(vector, i));
	}
}

void vector_free(struct Vector *vector) {
	free(vector->data);
}
