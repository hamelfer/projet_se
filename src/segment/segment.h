#ifndef SEGMENT__H
#define SEGMENT__H

#include "matrix.h"

#include <stddef.h>
#include <semaphore.h>

typedef struct segment segment_t;

segment_t *segment_init(size_t n, size_t m, size_t p);

int segment_get_lock_matrixA(segment_t *s);
int segment_get_lock_matrixB(segment_t *s);
int segment_get_lock_matrixC(segment_t *s);

int segment_release_lock_matrixA(segment_t *s);
int segment_release_lock_matrixB(segment_t *s);
int segment_release_lock_matrixC(segment_t *s);

matrix_t *segment_get_matrixA(segment_t *s);
matrix_t *segment_get_matrixB(segment_t *s);
matrix_t *segment_get_matrixC(segment_t *s);

#endif
