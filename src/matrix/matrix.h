#ifndef MATRIX__H
#define MATRIX__H

#include <stddef.h>

typedef struct matrix matrix_t;

// retourne la taille occupe par la matrice (structure et donnes)
size_t matrix_segmentSize(size_t nbLines, size_t nbColumns);

//initialise la matrice (structure)
int matrix_init(matrix_t *m, size_t nbLines, size_t nbColumns);

// retournent le nombre de lignes et colonnes
size_t matrix_get_nbLines(matrix_t *m);
size_t matrix_get_nbColumns(matrix_t *m);

//renvoie la cellule (ligne, column)
int *matrix_get_cell(matrix_t *m, size_t line, size_t column);

//ecrit la matrice dans le fichier de descripteur fd
int matrix_write(int fd, matrix_t *m);

//mutex lock et release
int matrix_get_lock(matrix_t *m);
int matrix_release_lock(matrix_t *m);

#if defined DEBUG && DEBUG != 0

void debug_matrix_t(matrix_t *m);
void debug_matrix_data(matrix_t *m);

#endif

#endif
