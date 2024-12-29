#include "workera.h"
#include "segment.h"
#include "workerb.h"

#include <stdio.h>
#include <stdlib.h>

#include <signal.h>

// Fonction de gestion des signaux
static void handle_signal(int sig);

segment_t *worker_a(size_t n, size_t m, size_t p, int maxInt) {
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
      if (signal(SIGINT, handle_signal) == SIG_ERR) {
        fprintf(stderr, "***Error : signal\n");
        exit(EXIT_FAILURE);
      }
      if (signal(SIGTERM, handle_signal) == SIG_ERR) {
          fprintf(stderr, "***Error : signal\n");
          exit(EXIT_FAILURE);
      }
      if (worker_b(s, maxInt) != 0) {
        exit(EXIT_FAILURE);
      }
      exit(EXIT_SUCCESS);
    }
  }
  srand((unsigned int) time(NULL));
  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      int *cell = matrix_get_cell(segment_get_matrixA(s), i, j);
      int randomInt = rand() % maxInt;
      *cell = maxInt;
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
