#define _POSIX_C_SOURCE 200112L

#include "segment.h"
#include "matrix.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

struct segment {
  size_t size;
  matrix_t *matrixA;
  matrix_t *matrixB;
  matrix_t *matrixC;
  void *raw;
};

segment_t *segment_init(size_t n, size_t m, size_t p) {
  int fd;
  {
    char *shm_name = get_pid_based_name(getpid());
    if ((fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
      perror("segment_init: shm_open");
      return NULL;
    }
    if (sem_unlink(shm_name) == -1) {
      perror("segment_init: shm_unlink");
    }
    free(shm_name);
  }
  size_t sizeMatrixA = matrix_segmentSize(n, m);
  size_t sizeMatrixB = matrix_segmentSize(m, p);
  size_t sizeMatrixC = matrix_segmentSize(n, p);
  size_t sizeSegmentHead = (size_t) ((sizeof(segment_t)));
  size_t totalSize = 0;
  {
     totalSize = sizeSegmentHead + sizeMatrixA + sizeMatrixB + sizeMatrixC;
    if (ftruncate(fd, (off_t) totalSize) != 0) {
      perror("segment_init: ftruncate");
      return NULL;
    }
  }
  segment_t *segPtr = mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (segPtr == MAP_FAILED) {
    perror("segment_init: mmap");
    return NULL;
  }
  segPtr->size = totalSize;
  segPtr->raw = (void *) (segPtr + 1);
  segPtr->matrixA = (matrix_t *) (segPtr->raw);
  segPtr->matrixB = (matrix_t *) (((char *) (segPtr->matrixA)) + sizeMatrixA);
  segPtr->matrixC = (matrix_t *) (((char *) (segPtr->matrixB)) + sizeMatrixB);
  if (matrix_init(segment_get_matrixA(segPtr), n, m) != 0) {
    return NULL;
  }
  if (matrix_init(segment_get_matrixB(segPtr), m, p) != 0) {
    return NULL;
  }
  if (matrix_init(segment_get_matrixC(segPtr), n, p) != 0) {
    return NULL;
  }
  return segPtr;
}

size_t segment_get_size(segment_t *s) {
  return s->size;
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

int segment_write_matrixes(int fd, segment_t *s) {
  size_t elementsSizeMatrixA = matrix_get_size(segment_get_matrixA(s));
  size_t elementsSizeMatrixB = matrix_get_size(segment_get_matrixB(s));
  size_t elementsSizeMatrixC = matrix_get_size(segment_get_matrixC(s));
  size_t elementsSizeMatrixes = elementsSizeMatrixA + elementsSizeMatrixB + elementsSizeMatrixC;
  int *buffer = malloc(elementsSizeMatrixes);
  if (buffer == NULL) {
    fprintf(stderr, "segment_write_matrixes: Out of memory\n");
    return -1;
  }
  int *intPtr = buffer;
  if (matrix_write(&intPtr, segment_get_matrixA(s)) != 0) {
    return -1;
  }
  if (matrix_write(&intPtr, segment_get_matrixB(s)) != 0) {
    return -1;
  }
  if (matrix_write(&intPtr, segment_get_matrixC(s)) != 0) {
    return -1;
  }
  size_t bytesToWrite = elementsSizeMatrixes;
  ssize_t r;
  char *ptr = (char *) buffer;
  while (bytesToWrite != 0 && (r = write(fd, ptr, bytesToWrite)) != (ssize_t) bytesToWrite) {
    if (r == -1) {
      perror("segment_write_matrixes: write");
      return -1;
    }
    bytesToWrite -= (size_t) r;
    ptr += r;
  }
  return 0;
}

#if defined DEBUG && DEBUG != 0

void debug_segment(segment_t *s) {
  fprintf(stderr, "\n======================== debug segment =======================\n");
  debug_matrix_t(s->matrixA);
  debug_matrix_t(s->matrixB);
  debug_matrix_t(s->matrixC);
  debug_matrix_data(s->matrixA);
  debug_matrix_data(s->matrixB);
  debug_matrix_data(s->matrixC);
  fprintf(stderr, "\n======================== end debug segment =======================\n");
}

#endif
