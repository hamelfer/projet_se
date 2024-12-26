#define _POSIX_C_SOURCE 200112L

#include "segment.h"
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

struct segment {
  matrix_t *matrixA;
  matrix_t *matrixB;
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
  int fd;
  {/*creation shm*/
    char *shm_name = get_shm_name();
    if ((fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
      perror("segment_init: shm_open");
      return NULL;
    }
    if (sem_unlink(shm_name) == -1) {
      //TODO5 : verifier pourquoi sem_unlink renvoie -1
      perror("segment_init: shm_unlink");
    }
    free(shm_name);
  }
  size_t sizeMatrixA = matrix_segmentSize(n, m);
  size_t sizeMatrixB = matrix_segmentSize(m, p);
  size_t sizeMatrixC = matrix_segmentSize(n, p);
  size_t sizeSegmentHead = (size_t) ((sizeof(segment_t)));
  size_t totalSize = 0;
  {/*adjusting size*/
    /*anough space for a segment_t*/
    /* + anough space for matrixes*/
     totalSize = sizeSegmentHead + sizeMatrixA + sizeMatrixB + sizeMatrixC;
    if (ftruncate(fd, (off_t) totalSize) != 0) {
      perror("segment_init: ftruncate");
      return NULL;
    }
  }
  /*mapping the shared memory*/
  segment_t *segPtr = mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (segPtr == MAP_FAILED) {
    perror("segment_init: mmap");
    return NULL;
  }
  /*setting segmentHead (segment_t)*/
  segPtr->raw = segPtr + sizeSegmentHead;
  segPtr->matrixA = (matrix_t *) (segPtr->raw);
  segPtr->matrixB = (matrix_t *) ((char *) (segPtr->matrixA) + sizeMatrixA);
  segPtr->matrixC = (matrix_t *) ((char *) (segPtr->matrixB) + sizeMatrixB);
  /*initialize matrixes*/
  if (matrix_init(segment_get_matrixA(segPtr), n, m) != 0) {
    //FIX : unmap mapped memory.
    return NULL;
  }
  if (matrix_init(segment_get_matrixB(segPtr), m, p) != 0) {
    //FIX : unmap mapped memory.
    return NULL;
  }
  if (matrix_init(segment_get_matrixC(segPtr), n, p) != 0) {
    //FIX : unmap mapped memory.
    return NULL;
  }
  return segPtr;
}

int segment_get_lock_matrixA(segment_t *s) {
  return matrix_get_lock(segment_get_matrixA(s));
}

int segment_get_lock_matrixB(segment_t *s) {
  return matrix_get_lock(segment_get_matrixB(s));
}

int segment_get_lock_matrixC(segment_t *s) {
  return matrix_get_lock(segment_get_matrixC(s));
}

int segment_release_lock_matrixA(segment_t *s) {
  return matrix_release_lock(segment_get_matrixA(s));
}

int segment_release_lock_matrixB(segment_t *s) {
  return matrix_release_lock(segment_get_matrixB(s));
}

int segment_release_lock_matrixC(segment_t *s) {
  return matrix_release_lock(segment_get_matrixC(s));
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
