#include "matrix.h"
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

struct matrix {
  sem_t lock;
  size_t nbLines;
  size_t nbColumns;
  int **lines;
};

static int get_random_int() {
  return 1;
}

size_t matrix_size(matrix_t *m) {
  return m->nbLines * m->nbColumns * sizeof(int);
}

void matrix_cell_set_random(matrix_t *m, size_t ligne, size_t column) {
  m->lines[ligne][column] = get_random_int();
}

int matrix_write(int fd, matrix_t *m) {
  if (m == NULL) {
    return -1;
  }
  size_t size = matrix_size(m);
  size_t written = 0;
  for (size_t i = 0; i < m->nbLines; ++i) {
    for (size_t j = 0; j < m->nbColumns; ++j) {
      ssize_t r = write(fd, &m->lines[i][j], sizeof(int));
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
