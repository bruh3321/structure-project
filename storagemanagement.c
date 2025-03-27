#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "storagemanagement.h"
#include "structs.h"

int chargerLivres(const char* filename) {
    FILE *fichier = fopen(filename, "r");
    if (!fichier) {
        return 0;
    }

    Livre l;
    int w=3,h=3, indicateur=1;
    while(fscanf(fichier, "%s %s %s %d %d %d", 
               l.code, l.titre, l.auteur, 
               &l.annee, &l.nbExemplaires, 
               &l.nbExemplairesDisponibles) != EOF) {
        // Logique d'affichage des livres
            mvprintw(w, h, "%d Code : %s \n Titre: %s \n Auteur %s \n Annee: %d \n Nombre d'exemplaire: %d \n Nombre d'exemplaire disp. :%d", 
               indicateur,l.code, l.titre, l.auteur, 
               l.annee, l.nbExemplaires, 
               l.nbExemplairesDisponibles);
            w+=7;
            indicateur++;
    }
    fclose(fichier);
    return 1;
}

int sauvegarderLivre(const char* filename, Livre *l) {
    FILE *fichier = fopen(filename, "a");
    if (!fichier) {
        return 0;
    }
    
    fprintf(fichier, "%s %s %s %d %d %d\n", 
           l->code, l->titre, l->auteur, 
           l->annee, l->nbExemplaires, 
           l->nbExemplairesDisponibles);
    
    fclose(fichier);
    return 1;
}


int rechercherLivre(const char* filename, const char* critere) {
    // Implémentez la recherche
    return 1;
}

int emprunterLivre(const char* filename, const char* cnie, const char* codeLivre) {
    // Logique d'emprunt
    return 1;
}

int rendreLivre(const char* filename, const char* codeLivre) {
    // Logique de retour
    return 1;
}