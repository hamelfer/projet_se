#include "worker.h"
#include "segment.h"
#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#include <pthread.h>

#include <sys/wait.h>

typedef struct compute {
  matrix_t *matrixA;
  matrix_t *matrixB;
  size_t line;
  size_t column;
  int *cell;
} compute_t;

// la fonction utilise par les threads pour le calcul des cellules de c
static void *cell_compute_fun(void *arg);

//fonction de generation d'un int aleatoire
static int get_random_int();

segment_t *worker_a(size_t n, size_t m, size_t p) {
  segment_t *s = segment_init(n, m, p);
  if (segment_get_lock_matrixA(s) != 0) {
    return NULL;
  }
  pid_t pid = fork();
  if (pid == -1) {
    perror("worker_a: fork");
    return NULL;
  }
  {
    if (pid == 0) {
      if (worker_b(s) != 0) {
        exit(EXIT_FAILURE);
      }
      exit(EXIT_SUCCESS);
    }
  }
  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      int *cell = matrix_get_cell(segment_get_matrixA(s), i, j);
      *cell = get_random_int();
    }
  }
  if (segment_release_lock_matrixA(s) != 0) {
    return NULL;
  }
  int wstatus;
  if (wait(&wstatus) == (pid_t) -1) {
    perror("worker_a: wait");
    return NULL;
  }
  if (WIFEXITED(wstatus)) {
    if (WEXITSTATUS(wstatus) == EXIT_FAILURE) {
      fprintf(stderr, "***Aborting: worker_a: child process failure\n");
      return NULL;
    }
  } else {
    fprintf(stderr, "***Aborting: worker_a: child process not terminated normally\n");
    return NULL;
  }
  return s;
}

int worker_b(segment_t *s) {
  matrix_t *matrixB = segment_get_matrixB(s);
  for (size_t i = 1; i <= matrix_get_nbLines(matrixB); ++i) {
    for (size_t j = 1; j <= matrix_get_nbColumns(matrixB); ++j) {
      int *cell = matrix_get_cell(matrixB, i, j);
      *cell = get_random_int();
    }
  }
  if (segment_get_lock_matrixA(s) != 0) {
    return -1;
  }
  matrix_t *matrixA = segment_get_matrixA(s);
  matrix_t *matrixC = segment_get_matrixC(s);
  for (size_t i = 1; i <= matrix_get_nbLines(matrixC); ++i) {
    for (size_t j = 1; j <= matrix_get_nbColumns(matrixC); ++j) {
      compute_t *arg = malloc(sizeof(compute_t));
      if (arg == NULL) {
        fprintf(stderr, "***Error: malloc: Out of memory.");
        return -1;
      }
      arg->matrixA = matrixA;
      arg->matrixB = matrixB;
      arg->line = i;
      arg->column = j;
      arg->cell = matrix_get_cell(matrixC, i, j);
      pthread_t thread_i_j;
      if (pthread_create(&thread_i_j, NULL, cell_compute_fun, (void *) arg) != 0) {
        fprintf(stderr, "***Error: pthread_create.");
        return -1;
      }
      compute_t *res;
      if (pthread_join(thread_i_j, (void **) &res) != 0) {
        fprintf(stderr, "***Error: pthread_join.");
        return -1;
      }
      free(res);
    }
  }
  if (segment_release_lock_matrixA(s) != 0) {
    return -1;
  }
  return 0;
}

int get_random_int() {
  return 1;
}

void *cell_compute_fun(void *arg) {
  compute_t *compute_arg = arg;
  matrix_t *matrixA = compute_arg->matrixA;
  matrix_t *matrixB = compute_arg->matrixB;
  size_t line = compute_arg->line;
  size_t column = compute_arg->column;
  int res = 0;
  for (size_t i = 1; i <= matrix_get_nbColumns(matrixA); ++i) {
    int *cellFromMatrixA = matrix_get_cell(matrixA, line, i);
    int *cellFromMatrixB = matrix_get_cell(matrixB, i, column);
    res += (*cellFromMatrixA) * (*cellFromMatrixB);
  }
  *compute_arg->cell = res;
  return arg;
}
