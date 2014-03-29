#ifndef VECTOR_H_
#define VECTOR_H_ 1

#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

struct Vector {
    int length;     // slots used so far
    int capacity;   // total available slots
    char **data;    // array of strings
};

void vector_init(struct Vector *vector);
int  vector_append(struct Vector *vector, char *value);
int  vector_append_copy(struct Vector *vector, const char *value, size_t l);
char* vector_get(struct Vector *vector, int index);
ssize_t vector_find(struct Vector *vector, const char value[]);
void vector_print(struct Vector *vector);
void vector_free(struct Vector *vector);

#endif
