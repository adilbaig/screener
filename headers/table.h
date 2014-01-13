#ifndef TABLE_H_
#define TABLE_H_ 1


#include <stdio.h>
#include "vector.h"

struct Table {
	int length;
	int capacity;
	struct Vector** vectors;
};

void table_init(struct Table *table);
void table_append(struct Table *table, struct Vector *vector);
void table_increase_capacity(struct Table *table, int capacity);
struct Vector* table_get(struct Table *table, size_t index);
ssize_t table_get_values(struct Table *table, const char *search, char ***strings);
void table_print(struct Table *table);
void table_free(struct Table *table);

#endif /* TABLE_H_ */
