#ifndef WORKER_B__H
#define WORKER_B__H

#include "segment.h"

// Fonction aui initialise la matrice B su segment pointee par s
//    avec des valeurs aleatoires ne depassant pas maxInt et qui calcule
//    la matrice C qui est le produit de la matrice A et B en
//    utilisant ((nombre de ligne de C) * (nombre de colonnes de C))
//    threads.
//  Renvoie 0 en cas de succes. Renvoie sinon -1.
int worker_b(segment_t *s, int maxInt);

#endif
