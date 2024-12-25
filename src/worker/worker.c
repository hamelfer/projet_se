#include "worker.h"
#include "segment.h"
#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

segment_t *worker_a(size_t n, size_t m, size_t p, int maxValue) {
  segment_t *s = segment_init(n, m, p);
  
  pid_t pid = fork();
  if (pid == -1) {
    perror("worker_a: fork");
    return NULL;
  }
  if (pid == 0) {
    int exit_status = EXIT_SUCCESS;
    if (worker_b(s) != 0) {
      exit_status = EXIT_FAILURE;
    }
    exit(exit_status);
  }
  fprintf(stderr, "matrix a initialized\n");
  fprintf(stderr, "waiting worker_b\n");
  return s;
}

int worker_b(segment_t *s) {
  sleep(5);
  fprintf(stderr, "matrix b initialized and c\n");
  fprintf(stderr, "workerb end\n");
  s = s;
  return 0;
}
