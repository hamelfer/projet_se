//  Module matrix
//  L'utilisateur est responsable de la consistance des parametres passes
//  aux fonctions de ce module.

#ifndef MATRIX__H
#define MATRIX__H

#include <stddef.h>

typedef struct matrix matrix_t;

// Retourne la taille necessaire a stocker une matrice de nbLines lignes
//    et nbColumns colonnes.
size_t matrix_segmentSize(size_t nbLines, size_t nbColumns);

// retourne la taille en octets occupee par les elements de la matrice
size_t matrix_get_size(matrix_t *m);

//  Initialise la matrice pointee par m et qui a nbLignes lignes et nbColumns
//    colonnes.
int matrix_init(matrix_t *m, size_t nbLines, size_t nbColumns);

// Retourne le nombre de lignes de la matrice pointee par m.
size_t matrix_get_nbLines(matrix_t *m);

// Retourne le nombre de colonnes de la matrice pointee par m.
size_t matrix_get_nbColumns(matrix_t *m);

//  Renvoie un pointeur vers la cellule de ligne line et colonne column
//    de la matrice pointee par m.
int *matrix_get_cell(matrix_t *m, size_t line, size_t column);

// Copie sequenciellement les elements de la matrice pointee par m dans
//  la zone memoire pointee par *ptr.
//  Renvoie 0 et fait pointer *ptr a l'adresse du dernier element ecrit + 1.
int matrix_write(int **ptr, matrix_t *m);

// Verrouille l'acces a la matrice pointee par m. Cet appel est bloquant
//    si le verroux n'est pas disponible immediatement.
int matrix_get_lock(matrix_t *m);

// Deverouille l'acces a la metrice pointee par m.
int matrix_release_lock(matrix_t *m);

#if defined DEBUG && DEBUG != 0

// Fonction d'inspection de la structure matrix_t, accessible avec
//    DEBUG > 1
void debug_matrix_t(matrix_t *m);

// Fonction d'inspection des elements de la matrice, accessible avec DEBUG != 0.
void debug_matrix_data(matrix_t *m);

#endif

#endif
