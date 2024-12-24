#define _POSIX_C_SOURCE 200112L

#include "segment.h"
#include "../matrix/matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

struct segment {
  sem_t lock_seg;
  sem_t lock_matrixA;
  matrix_t *matrixA;
  sem_t lock_matrixB;
  matrix_t *matrixB;
  sem_t lock_matrixC;
  matrix_t *matrixC;
  void *raw;
};

char *get_shm_name() {
  int pid = (int) getpid();
  size_t strSize = (size_t) snprintf(NULL, 0, "%d", pid);
  char *str = malloc(strSize + 1);
  snprintf(str, strSize, "%d", pid);
  return str;
}

segment_t *segment_init(size_t n, size_t m, size_t p) {
  size_t size = (size_t) ((sizeof(int) * (n * m + m * p + n * p))
        + (4 * sizeof(sem_t))
        + sizeof(void *));
  int fd;
  if ((fd = shm_open(get_shm_name(), O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
    perror("segment_init: shm_open");
    return NULL;
  }
  if (sem_unlink(get_shm_name()) == -1) {
    perror("segment_init: shm_unlink");
  }
  if (ftruncate(fd, (off_t) size) != 0) {
    perror("segment_init: ftruncate");
    return NULL;
  }
  segment_t *segPtr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (segPtr == MAP_FAILED) {
    perror("segment_init: mmap");
    return NULL;
  }

  /*initialize semaphores*/
  if (sem_init(segment_get_sem_matrixA(segPtr), 1, 1)) {
    perror("segment_init: sem_init");
    //FIX : free allocated memory.
    return NULL;
  }
  if (sem_init(segment_get_sem_matrixB(segPtr), 1, 1)) {
    perror("segment_init: sem_init");
    //FIX : free allocated memory.
    return NULL;
  }
  if (sem_init(segment_get_sem_matrixC(segPtr), 1, 1)) {
    perror("segment_init: sem_init");
    //FIX : free allocated memory.
    return NULL;
  }
  return segPtr;
}

sem_t *segment_get_sem_matrixA(segment_t *s) {
  return &s->lock_matrixA;
}

sem_t *segment_get_sem_matrixB(segment_t *s) {
  return &s->lock_matrixB;
}

sem_t *segment_get_sem_matrixC(segment_t *s) {
  return &s->lock_matrixC;
}

matrix_t *segment_get_matrixA(segment_t *s) {
  return s->matrixA;
}

matrix_t *segment_get_matrixB(segment_t *s) {
  return s->matrixB;
}

matrix_t *segment_get_matrixC(segment_t *s) {
  return s->matrixC;
}
