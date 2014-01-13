#include "../headers/commands.h"

ssize_t is_column(struct Table* table, char* column)
{
    struct Vector* vector = table_get(table, 0);
    return vector_find(vector, column);
}

void print_columns(struct Table *table)
{
    struct Vector* vector = table_get(table, 0);

    for(int i = 0; i < vector->length; i++) {
        printf("'%s' ", vector->data[i]);
    }

    printf("\n");
}

void print_column(struct Table* table, char* column)
{
    char **rez;

    // CAREFUL! Every time table_get_values > 0, it has malloced. Free it!
    if (table_get_values(table, column, &rez) >= 0) {
        for (int i = 0; i < table->length - 1; i++) {
            printf("'%s', ", rez[i]);
        }

        free(rez);
        printf("\n");
    }
}
