#ifndef WORKER__H
#define WORKER__H

#include <stddef.h>
#include "segment.h"

segment_t *worker_a(size_t n, size_t m, size_t p, int maxValue);
int worker_b(segment_t *s);

#endif
