#include "matrix.h"
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// Macro définie dans Large File Support extension (LFS)
#ifndef _FILE_OFFSET_BITS
// Sur les machines 32 bits, utilise 64 bits pour off_t
#define _FILE_OFFSET_BITS 64
#endif

struct matrix {
  sem_t lock;
  size_t nbLines;
  size_t nbColumns;
  int *data;
};

int matrix_truncate(int fd, size_t nbLines, size_t NbCols, size_t *sizePtr) {
  size_t matrixSize = sizeof(matrix_t) + (sizeof(int) * (nbLines * NbCols));
  *sizePtr = 0;
  if (ftruncate(fd, (off_t) matrixSize) != 0) {
    perror("matrix_truncate: ftruncate");
    return -1;
  }
  *sizePtr += matrixSize;
  return 0;
}

int matrix_init(matrix_t *m, size_t nbLines, size_t nbColumns) {
  m->nbLines = nbLines;
  m->nbColumns = nbColumns;
  if (sem_init(&m->lock, 1, 1) == -1) {
    perror("matrix_init: sem_init");
    return -1;
  }
  m->data = (int *) (m + sizeof(matrix_t));
  return 0;
}

static int get_random_int() {
  return 1;
}

size_t matrix_size(matrix_t *m) {
  return m->nbLines * m->nbColumns * sizeof(int);
}

int matrix_cell_set_random(matrix_t *m, size_t line, size_t column) {
  if (line <= 0 || column <= 0 || line > m->nbLines || column > m->nbColumns) {
    fprintf(stderr, "***Error: matrix_cell_set_random: invalid matrix index");
    return -1;
  }
  size_t index = ((line - 1) * m->nbColumns) + column;
  m->data[index] = get_random_int();
  return 0;
}

int matrix_write(int fd, matrix_t *m) {
  if (m == NULL) {
    return -1;
  }
  size_t size = matrix_size(m);
  size_t written = 0;
  for (size_t i = 0; i < m->nbLines; ++i) {
    for (size_t j = 0; j < m->nbColumns; ++j) {
      size_t index = (i * m->nbLines) + j;
      ssize_t r = write(fd, &m->data[index], sizeof(int));
      if (r == (ssize_t) -1) {
        perror("matrix_write: write");
        return -1;
      }
      if (r != sizeof(int)) {
        fprintf(stderr, "***Error: matrix_write: write: %zu bytes are not written.", size - written);
        return -1;
      }
      written += sizeof(int);
    }
  }
  return 0;
}

int matrix_get_lock(matrix_t *m) {
  if (sem_wait(&m->lock) != 0) {
    perror("matrix_get_lock: sem_wait");
    return -1;
  }
  return 0;
}

int matrix_release_lock(matrix_t *m) {
  if (sem_post(&m->lock) != 0) {
    perror("matrix_release_lock: sem_post");
    return -1;
  }
  return 0;
}
