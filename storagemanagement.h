#include "structs.h"
#ifndef STORAGE_H
#define STORAGE_H
#define MAX_LIVRES 1000
#define MAX_ETUDIANTS 1000

int chargerLivres(const char* filename, Livre* livres, int* total);
int sauvegarderLivre(const char* filename, Livre* l);
Livre* rechercherLivres(const char* filename, const char* critere, int type, int* nbTrouves);
int emprunterLivre(Etudiant* etudiant, const char* codeLivre);
int rendreLivre(Etudiant* etudiant, const char* codeLivre);
int chargerEtudiant(const char* filename,Etudiant* etudiant, int *total);
#endif