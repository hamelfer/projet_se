//  Module segment
//

#ifndef SEGMENT__H
#define SEGMENT__H

#include "matrix.h"

#include <stddef.h>
#include <semaphore.h>

// Type est nom d'une structure servant a gerer un segment de memoire
//    partagee conforme aux exigences du projet.
typedef struct segment segment_t;

// Initialise un segment de memoire partagee pour gerer une matrice
//    A de n lignes et m colonnes, une matrice B de m lignes et p
//    colonnes et une matrice C de n lignes et p colonnes.
segment_t *segment_init(size_t n, size_t m, size_t p);

//  Renvoie la taille occupe par le segment de memoire partagee.
size_t segment_get_size(segment_t *s);

// Verrouillage de la matrice A du segment pointe par s.
//    l'appel a cette fonction est bloquant si le verroux n'est pas
//    disponible immediatement.
int segment_get_lock_matrixA(segment_t *s);

// Verrouillage de la matrice B du segment pointe par s.
//    l'appel a cette fonction est bloquant si le verroux n'est pas
//    disponible immediatement.
int segment_get_lock_matrixB(segment_t *s);

// Verrouillage de la matrice C du segment pointe par s.
//    l'appel a cette fonction est bloquant si le verroux n'est pas
//    disponible immediatement.
int segment_get_lock_matrixC(segment_t *s);

//  Deverouillage de la matrice prealablement verouillee A du segment
//    pointe par s.
int segment_release_lock_matrixA(segment_t *s);

//  Deverouillage de la matrice prealablement verouillee B du segment
//    pointe par s.
int segment_release_lock_matrixB(segment_t *s);

//  Deverouillage de la matrice prealablement verouillee C du segment
//    pointe par s.
int segment_release_lock_matrixC(segment_t *s);

// Renvoie la matrice A du segment pointe par s.
matrix_t *segment_get_matrixA(segment_t *s);

// Renvoie la matrice B du segment pointe par s.
matrix_t *segment_get_matrixB(segment_t *s);

// Renvoie la matrice C du segment pointe par s.
matrix_t *segment_get_matrixC(segment_t *s);

//ecrit les matrices A, B et C dans le fichier correspondant a fd
int segment_write_matrixes(int fd, segment_t *s);

#if defined DEBUG && DEBUG != 0

// Fonction d'inspection du segment pointe par s, accessible avec
//    DEBUG != 0
void debug_segment(segment_t *s);

#endif

#endif
