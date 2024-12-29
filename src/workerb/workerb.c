#include <pthread.h>
#include "workerb.h"
#include "segment.h"
#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_INT_R 100

// type et nom d'une structure servant a gerer l'argument de la
//  fonction passee aux threads de calcul
typedef struct compute {
  matrix_t *matrixA;
  matrix_t *matrixB;
  size_t line;
  size_t column;
  int *cell;
} compute_t;

// type et nom d'une structure servant a construire une liste chainee
//  dont les elements sont de type pthread_t.
typedef struct thread_node {
    pthread_t thread_id;
    struct thread_node *next;
} thread_node_t;

// Pointeur vers la tête de la liste chaînée
static thread_node_t *thread_list_head = NULL;

// la fonction utilise par les threads pour le calcul des cellules de c
static void *cell_compute_fun(void *arg);

// Fonction pour ajouter un thread à la liste
static void add_thread(pthread_t thread_id);

// Fonction pour rejoindre tous les threads dans la liste
static void join_all_threads();

int worker_b(segment_t *s, int maxInt) {
  matrix_t *matrixB = segment_get_matrixB(s);
  srand((unsigned int) time(NULL));
  for (size_t i = 1; i <= matrix_get_nbLines(matrixB); ++i) {
    for (size_t j = 1; j <= matrix_get_nbColumns(matrixB); ++j) {
      int *cell = matrix_get_cell(matrixB, i, j);
      int randomInt = rand() % maxInt;
      *cell = randomInt;
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
      add_thread(thread_i_j);
    }
  }
  join_all_threads();
  if (segment_release_lock_matrixA(s) != 0) {
    return -1;
  }
  return 0;
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

static void add_thread(pthread_t thread) {
  thread_node_t *p = malloc(sizeof(thread_node_t));
  if (p == NULL) {
    fprintf(stderr, "***Error: malloc: Out of memory.");
    exit(EXIT_FAILURE);
  }
  p->thread_id = thread;
  p->next = thread_list_head;
  thread_list_head = p;
}

static void join_all_threads() {
  thread_node_t *p = thread_list_head;
  while (p != NULL) {
    pthread_join(p->thread_id, NULL);
    thread_node_t *temp = p;
    p = p->next;
    free(temp);
  }
  thread_list_head = NULL;
}
