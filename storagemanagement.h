#include "structs.h"
#ifndef STORAGE_H
#define STORAGE_H
#define MAX_LIVRES 1000

int chargerLivres(const char* filename, Livre* livres, int* total);
int sauvegarderLivre(const char* filename, Livre* l);
int rechercherLivre(const char* filename, const char* critere);
int emprunterLivre(const char* filename, const char* cnie, const char* codeLivre);
int rendreLivre(const char* filename, const char* codeLivre);

#endif