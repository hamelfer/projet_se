#ifndef MATRIX__H
#define MATRIX__H

#include <stddef.h>

typedef struct matrix matrix_t;

int matrix_truncate(int fd, size_t nbLines, size_t NbCols, size_t *sizePtr);
int matrix_init(matrix_t *m, size_t nbLines, size_t nbColumns);

size_t matrix_size(matrix_t *m);
int matrix_cell_set_random(matrix_t *m, size_t line, size_t column);
int matrix_write(int fd, matrix_t *m);

int matrix_get_lock(matrix_t *m);
int matrix_release_lock(matrix_t *m);

#endif
