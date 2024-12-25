#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "segment.h"
#include "matrix.h"
#include "worker.h"



#define MAIN_TUBE_PATHNAME "server_main_tube"

int main() {
/*
  int mainTube[2];
  if (mkfifo(MAIN_TUBE_PATHNAME, (mode_t) 0666) != 0) {
    perror("mkfifo");
    return EXIT_FAILURE;
  }
  if (open(MAIN_TUBE_PATHNAME, O_RDWR)) {
    
  }
*/
  int i = 1;
  while (i > 0) {
    fprintf(stderr, "request\n");
    segment_t *s = worker_a(2, 3, 3, 2);
    if (s == NULL) {
      return EXIT_FAILURE;
    }
    int fd = STDOUT_FILENO;
    matrix_write(fd, segment_get_matrixC(s));
    fprintf(stderr, "matrix wrote to stout\n");
    fprintf(stderr, "waiting next request\n");
    --i;
  }
  fprintf(stderr, "exit\n");
  return EXIT_SUCCESS;
}
