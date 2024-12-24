#ifndef SEGMENT__H
#define SEGMENT__H

#include "matrix.h"

#include <stddef.h>
#include <semaphore.h>

typedef struct segment segment_t;

segment_t *segment_init(size_t n, size_t m, size_t p);

sem_t *segment_get_sem_matrixA(segment_t *s);
sem_t *segment_get_sem_matrixB(segment_t *s);
sem_t *segment_get_sem_matrixC(segment_t *s);

matrix_t *segment_get_matrixA(segment_t *s);
matrix_t *segment_get_matrixB(segment_t *s);
matrix_t *segment_get_matrixC(segment_t *s);

#endif
