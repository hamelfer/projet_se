#ifndef WORKER__H
#define WORKER__H

#include <stddef.h>
#include "segment.h"

// fonction qui fait le travail de workera du projet et cree un processus enfant
//   qui va gerer le travail de workerb
segment_t *worker_a(size_t n, size_t m, size_t p);

// fonction qui fait le travail de workerb du projet
int worker_b(segment_t *s);

#endif
