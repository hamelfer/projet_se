#ifndef WORKER_A__H
#define WORKER_A__H

#include "segment.h"
#include <stddef.h>

// Fonction qui cree un segment et initialise la matrice A avec des
//    valeurs aleatoires ne depassant pas maxInt et qui lance un
//    processus de type workerb, puis renvoie le le segment une fois
//    le processus cree se termine.
// En cas de succes renvoie un pointeur vers le segment.
// Renvoie sinon NULL.
segment_t *worker_a(size_t n, size_t m, size_t p, int maxInt);

#endif
