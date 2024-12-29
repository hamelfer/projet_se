#ifndef WORKER_A__H
#define WORKER_A__H

#include "segment.h"
#include <stddef.h>

// fonction qui fait le travail de workera du projet et cree un processus enfant
//   qui va gerer le travail de workerb
segment_t *worker_a(size_t n, size_t m, size_t p, int maxInt);

#endif
