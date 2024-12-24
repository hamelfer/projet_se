#ifndef MATRIX__H
#define MATRIX__H

#include <stddef.h>

typedef struct matrix matrix_t;

size_t matrix_size(matrix_t *m);
void matrix_cell_set_random(matrix_t *m, size_t ligne, size_t column);
int matrix_write(int fd, matrix_t *m);

#endif
