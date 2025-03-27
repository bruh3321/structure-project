#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "storagemanagement.h"
#include "structs.h"

int chargerLivres(const char* filename, Livre *livres, int *total) {
    *total = 0;
    FILE *fichier = fopen(filename, "r");
    if (!fichier) return 0;

    char line[256];
    while(fgets(line, sizeof(line), fichier) && *total < MAX_LIVRES) {
        if(sscanf(line, "%s %s %s %d %d %d", 
               livres[*total].code, 
               livres[*total].titre, 
               livres[*total].auteur,
               &livres[*total].annee, 
               &livres[*total].nbExemplaires, 
               &livres[*total].nbExemplairesDisponibles) == 6) {
            (*total)++;
        }
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
    FILE *fichier = fopen("emprunt.txt", "r+");

    FILE *tmp = fopen("tmp.txt", "a+");
    Livre livre;
    if (!fichier) return 0;

    while (feof(fichier)){
        fscanf(fichier, "%s %s %s %d %d %d", 
               livre.code, 
               livre.titre, 
               livre.auteur,
               &livre.annee, 
               &livre.nbExemplaires, 
               &livre.nbExemplairesDisponibles);
        if (strcmp(livre.code, codeLivre) != 0){ 
           fprintf(tmp, "%s %s %s %d %d %d\n", 
               livre.code, 
               livre.titre, 
               livre.auteur,
               livre.annee, 
               livre.nbExemplaires, 
               livre.nbExemplairesDisponibles);
        }else{
            fprintf(tmp, "%s %s %s %d %d %d\n", 
                livre.code, 
                livre.titre, 
                livre.auteur,
                livre.annee, 
                livre.nbExemplaires, 
                livre.nbExemplairesDisponibles--);
        }
        remove("emprunt.txt");
        rename("tmp.txt", "emprunt.txt");
        fclose(tmp);   
    }
    

    return 1;
}

int rendreLivre(const char* filename, const char* codeLivre) {
    // Logique de retour
    return 1;
}