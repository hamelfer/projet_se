#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "utils.h"

int main() {
  int requests = open("server_request_pipe", O_WRONLY);
  if (requests == -1) {
    perror("open");
    return EXIT_FAILURE;
  }
  printf("request pipe opened\n");
  pid_t pid = getpid();
  int maxValue = 3;
  size_t nbLinesMatrixA = 2;
  size_t nbColumnsMatrixA = 2;
  size_t nbLinesMatrixB = 2;
  size_t nbColumnsMatrixB = 2;
  size_t bufferSize = sizeof(pid_t) + sizeof(int) + 4 * sizeof(size_t);
  char buffer[sizeof(pid_t) + sizeof(int) + 4 * sizeof(size_t)];
  pid_t *wPid = (pid_t *) buffer;
  int *wMaxValue = (int *) (wPid + 1);
  size_t *wNbLinesMatrixA = (size_t *) (wMaxValue + 1);
  size_t *wNbColumnsMatrixA = wNbLinesMatrixA + 1;
  size_t *wNbLinesMatrixB = wNbColumnsMatrixA + 1;
  size_t *wNbColumnsMatrixB = wNbLinesMatrixB + 1;
  *wPid = pid;
  *wMaxValue = maxValue;
  *wNbLinesMatrixA = nbLinesMatrixA;
  *wNbColumnsMatrixA = nbColumnsMatrixA;
  *wNbLinesMatrixB = nbLinesMatrixB;
  *wNbColumnsMatrixB = nbColumnsMatrixB;
  ssize_t r;
  size_t bytesToWrite = bufferSize;
  char *ptr = buffer;
  printf("client writing\n");
  printf("%zu / %zu\n", bytesToWrite, bufferSize);
  while (bytesToWrite != 0 && (r = write(requests, buffer, bytesToWrite)) != (ssize_t) bytesToWrite) {
    printf("%zu / %zu\n", bytesToWrite, bufferSize);
    if (r == -1) {
      perror("write");
      return EXIT_FAILURE;
    }
    bytesToWrite -= (size_t) r;
    ptr = ptr + r;
    fprintf(stderr, "wrote %zu / %zu\n", (size_t) r, bytesToWrite);
    return EXIT_FAILURE;
  }
  char *pipe_name = get_pid_based_name(getpid());
  int fd;
  if (mkfifo(pipe_name, 0666) == -1) {
    perror("mkfifo");
    return EXIT_FAILURE;
  }
  {
    if ((fd = open(pipe_name, O_RDONLY)) == -1) {
      perror("open");
      return EXIT_FAILURE;
    }
    printf("response pipe : %s opened\n", pipe_name);
    ssize_t r;
    size_t sa = sizeof(int) * nbLinesMatrixA * nbColumnsMatrixA;
    size_t sb = sizeof(int) * nbLinesMatrixB * nbColumnsMatrixB;
    size_t sc = sizeof(int) * nbLinesMatrixA * nbColumnsMatrixB;
    size_t bytesToRead = sa + sb + sc;
    size_t totalToRead = sa + sb + sc;
    int *ptrBuffer = calloc(totalToRead, 1);
    {
      char *ptr = (char *) ptrBuffer;
      printf("client reading\n");
      while (bytesToRead != 0 && (r = read(fd, ptr, bytesToRead)) != (ssize_t) totalToRead) {
        if (r == -1) {
          perror("read");
          return EXIT_FAILURE;
        }
        printf("%zu / %zu\n", bytesToRead, totalToRead);
        bytesToRead -= (size_t) r;
        ptr += (size_t) r;
      }
    }
    int *ptr = ptrBuffer;
    printf("matrix A:\n");
    for(size_t i = 0; i < nbLinesMatrixA; ++i) {
      for (size_t j = 0; j < nbColumnsMatrixA; ++j) {
        printf("| %d ", ptr[i * nbColumnsMatrixA + j]);
      }
      printf("|\n");
    }
    ptr += (nbLinesMatrixA * nbColumnsMatrixA);
    printf("matrix B:\n");
    for(size_t i = 0; i < nbLinesMatrixB; ++i) {
      for (size_t j = 0; j < nbColumnsMatrixB; ++j) {
        printf("| %d ", ptr[i * nbColumnsMatrixB + j]);
      }
      printf("|\n");
    }
    ptr += (nbLinesMatrixB * nbColumnsMatrixB);
    printf("matrix C:\n");
    for(size_t i = 0; i < nbLinesMatrixA; ++i) {
      for (size_t j = 0; j < nbColumnsMatrixB; ++j) {
        printf("| %d ", ptr[i * nbColumnsMatrixB + j]);
      }
      printf("|\n");
    }
    free(ptrBuffer);
  }
  if (close(fd) != 0) {
    perror("close");
    return EXIT_FAILURE;
  }
  if (unlink(pipe_name) != 0) {
    perror("unlink");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
