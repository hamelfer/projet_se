#ifndef MATRIX__H
#define MATRIX__H

#include <stddef.h>

typedef struct matrix matrix_t;

int matrix_init(matrix_t *m, size_t nbLines, size_t nbColumns);
// retourne la taille occupe par la matrice (structure et donnes)
size_t matrix_segmentSize(size_t nbLines, size_t nbColumns)


size_t matrix_get_nbLines(matrix_t *m);
size_t matrix_get_nbColumns(matrix_t *m);
int *matrix_get_cell(matrix_t *m, size_t line, size_t column);


//ecrit la matrice dans le fichier de descripteur fd
int matrix_write(int fd, matrix_t *m);

int matrix_get_lock(matrix_t *m);
int matrix_release_lock(matrix_t *m);

#endif
