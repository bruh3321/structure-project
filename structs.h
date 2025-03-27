#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    char code[10];
    char titre[50];
    char auteur[50];
    int annee;
    int nbExemplaires;
    int nbExemplairesDisponibles;
} Livre;

typedef struct {
    char prenom[30];
    char nom[30];
    char CNIE[30];
    char emtprunts[10][10]; // 10 emprunts de 10 caractères
} Etudiant;

#endif