#ifndef UTILS__H
#define UTILS__H

//  Renvoie une chaine de caracteres stockee dans le tas
//    basee sur le pid de l'appelant.
//  Un appel a free avec comme argument l'adresse retournee
//    peut etre envisage.
char *get_pid_based_name();

#endif
