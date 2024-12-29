#ifndef SEGMENT__H
#define SEGMENT__H

#include "matrix.h"

#include <stddef.h>
#include <semaphore.h>

typedef struct segment segment_t;

segment_t *segment_init(size_t n, size_t m, size_t p);

//renvoie la taille occupe par le segment de memoire partagee
size_t segment_get_size(segment_t *s);

// lock et unlock pour les matrices
int segment_get_lock_matrixA(segment_t *s);
int segment_get_lock_matrixB(segment_t *s);
int segment_get_lock_matrixC(segment_t *s);
int segment_release_lock_matrixA(segment_t *s);
int segment_release_lock_matrixB(segment_t *s);
int segment_release_lock_matrixC(segment_t *s);

//renvoie la matrice A
matrix_t *segment_get_matrixA(segment_t *s);

//renvoie la matrice A
matrix_t *segment_get_matrixB(segment_t *s);

//renvoie la matrice A
matrix_t *segment_get_matrixC(segment_t *s);

//ecrit les matrices A, B et C dans le fichier correspondant a fd
int segment_write_matrixes(int fd, segment_t *s);

#if defined DEBUG && DEBUG != 0

void debug_segment(segment_t *s);

#endif

#endif
