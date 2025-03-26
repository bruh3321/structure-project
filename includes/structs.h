#include <stdio.h>

typedef struct{
    char code[10];
    char titre[50];
    char auteur[50];
    int annee;
    int nbExemplaires;
    int nbExemplairesDisponibles;
    livre *suivant;
}livre;

typedef struct{
    char prenom[30];
    char nom[30];
    char CNIE[30];
//    char num[30];
    etudiant *suivant;
}etudiant;

