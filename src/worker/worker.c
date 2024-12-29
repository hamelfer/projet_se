#include "worker.h"
#include "segment.h"
#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#include <pthread.h>

#include <sys/wait.h>

#include <signal.h>

#define MAX_INT_R 100

typedef struct compute {
  matrix_t *matrixA;
  matrix_t *matrixB;
  size_t line;
  size_t column;
  int *cell;
} compute_t;

//=============================================================
// Définition de la structure (DONE)
typedef struct thread_node {
    pthread_t thread_id;
    struct thread_node *next;
} thread_node_t;

// Fonction pour ajouter un thread à la liste
static void add_thread(pthread_t thread_id);

// Fonction pour rejoindre tous les threads dans la liste
static void join_all_threads();

// Pointeur vers la tête de la liste chaînée de threads
static thread_node_t *thread_list_head = NULL;

// ========================================================

// la fonction utilise par les threads pour le calcul des cellules de c
static void *cell_compute_fun(void *arg);

//fonction de generation d'un int aleatoire
static int get_random_int();

// =======================================================================================
// Fonction de gestion des signaux
static void handle_signal(int sig);

static void handle_signal(int sig) {
  fprintf(stderr, "Signal %d\n", sig);
    
  // Rejoint tous les threads pour s'assurer qu'ils sont terminés proprement
  join_all_threads();
  exit(EXIT_SUCCESS);
}
//========================================================================================

segment_t *worker_a(size_t n, size_t m, size_t p) {
  segment_t *s = segment_init(n, m, p);
  /*lock matrixA*/
  if (segment_get_lock_matrixA(s) != 0) {
    return NULL;
  }
  pid_t pid = fork();
  if (pid == -1) {
    perror("worker_a: fork");
    return NULL;
  }
  {/*in child process*/
    if (pid == 0) {
      //TODO5 : gestion des signaux worker_b (DONE)
      //==================================================
      if (signal(SIGINT, handle_signal) == SIG_ERR) {
        fprintf(stderr, "Error : signal\n");
        exit(EXIT_FAILURE);
      }
      if (signal(SIGTERM, handle_signal) == SIG_ERR) {
          fprintf(stderr, "Error : signal\n");
          exit(EXIT_FAILURE);
      }
      //==================================================
      if (worker_b(s) != 0) {
        exit(EXIT_FAILURE);
      }
      exit(EXIT_SUCCESS);
    }
  }
  /*initialisation matrixA*/
  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      int *cell = matrix_get_cell(segment_get_matrixA(s), i, j);
      *cell = get_random_int();
    }
  }
  if (segment_release_lock_matrixA(s) != 0) {
    return NULL;
  }
  /*wait child process*/
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
  /*initialisation matrixB*/
  matrix_t *matrixB = segment_get_matrixB(s);
  for (size_t i = 1; i <= matrix_get_nbLines(matrixB); ++i) {
    for (size_t j = 1; j <= matrix_get_nbColumns(matrixB); ++j) {
      int *cell = matrix_get_cell(matrixB, i, j);
      *cell = get_random_int();
    }
  }
  /*calcul de matrixC*/
  if (segment_get_lock_matrixA(s) != 0) {
    return -1;
  }
  matrix_t *matrixA = segment_get_matrixA(s);
  matrix_t *matrixC = segment_get_matrixC(s);
  /*creation des threads de calcul*/
  for (size_t i = 1; i <= matrix_get_nbLines(matrixC); ++i) {/*lines*/
    for (size_t j = 1; j <= matrix_get_nbColumns(matrixC); ++j) {/*columns*/
      /*creation et initialisation de l'argument arg*/
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
      /*creation du thread de calcul de la cellule (i,j)*/
      pthread_t thread_i_j;
      if (pthread_create(&thread_i_j, NULL, cell_compute_fun, (void *) arg) != 0) {
        fprintf(stderr, "***Error: pthread_create.");
        //TODO5 : check
        return -1;
      }
      //TODO5 : store thread in waiting queue and not try to join immediately(DONE)
      // ICI JE COMPRENDS PAS POURQUOI TU AS FAIT ÇA
      /*compute_t *res;
      if (pthread_join(thread_i_j, (void **) &res) != 0) {
        fprintf(stderr, "***Error: pthread_join.");
        return -1;
      }
      free(res);
      */
      add_thread(thread_i_j);
    }
  }

  //TODO5 : attente des threads de calcul
  join_all_threads();
  if (segment_release_lock_matrixA(s) != 0) {
    return -1;
  }
  return 0;
}

// /================================================

// Variable globale pour la graine
static unsigned int seed;
pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;

// Fonction pour initialiser la graine
void init_random_seed() {
    pthread_mutex_lock(&s_mutex);
    seed = (unsigned int)time(NULL);
    pthread_mutex_unlock(&s_mutex);
}

int get_random_int() {
  //TODO5 : randomly generated result (DONE)
  unsigned int local_seed;
  unsigned int value_r;

  pthread_mutex_lock(&s_mutex);
  local_seed = seed;
  rand_r(&local_seed);
  value_r = local_seed % (unsigned int)(MAX_INT_R + 1);
  seed = local_seed;
  pthread_mutex_unlock(&s_mutex);

  return (int)value_r;
}

// ==============================================

void *cell_compute_fun(void *arg) {
  //TODO1 : add assertions for matrixA and matrixB (DONE)
  compute_t *compute_arg = arg;
  //==========================================================
    if(compute_arg->matrixA == NULL 
      || compute_arg->matrixB != NULL){
      fprintf(stderr, "Invalid arg\n");
      return NULL;
    }
  //==========================================================
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

//=============================================
//TODO5 : creation d'un module pour les listes chainees (enregistrer les ID de threads crees)

static void add_thread(pthread_t thread) {
    thread_node_t *p = malloc(sizeof(thread_node_t));
    if (p == NULL) {
        fprintf(stderr, "Error: malloc");
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

//===============================================