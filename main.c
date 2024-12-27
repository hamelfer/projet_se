#include <stdio.h>
#include <stdlib.h>

#include "segment.h"
#include "worker.h"

int main() {
  /*main version de test*/
  /*appel a la fonction worker_a bloquant*/
  segment_t *s = worker_a(2, 3, 3);
  if (s == NULL) {
    fprintf(stderr, "\nmain failure\n");
    return EXIT_FAILURE;
  }
  debug_segment(s);

  /*ecriture du resultat*/
  //int fd = ;
  //matrix_write(fd, segment_get_matrixC(s));
  return EXIT_SUCCESS;
}

//TODO5 : gestion des signaux worker_a
