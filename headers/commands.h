/*
 * commands.h
 *
 * Various commands that the server supports
 *
 *  Created on: 13-Jan-2014
 *      Author: adil
 */

#include <sys/types.h>
#include "table.h"

ssize_t is_column(struct Table* table, char* column);
void print_columns(struct Table *table);
void print_column(struct Table* table, char* column);
