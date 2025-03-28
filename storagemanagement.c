#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
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


int rechercherLivre(const char* filename, const char* critere, const int type) {
    // TO-DO : Implémentez la recherche
    return 1;
}

int emprunterLivre(Etudiant* etudiant, const char* codeLivre) {
    // livre emprunt section
    FILE *fichier = fopen("livres.txt", "r");

    FILE *tmp = fopen("tmp.txt", "w");
    Livre livre;
    if (!fichier) return 1;
    int state = 0;
    while (!feof(fichier)){
        fscanf(fichier, "%s %s %s %d %d %d\n", 
               livre.code, 
               livre.titre, 
               livre.auteur,
               &livre.annee, 
               &livre.nbExemplaires, 
               &livre.nbExemplairesDisponibles);
        if(strcmp(livre.code, codeLivre) == 0){ 
                if(livre.nbExemplairesDisponibles > 0){
                    fprintf(tmp, "%s %s %s %d %d %d\n", 
                        livre.code, 
                        livre.titre, 
                        livre.auteur,
                        livre.annee, 
                        livre.nbExemplaires, 
                        --livre.nbExemplairesDisponibles
                        );
                }else{
                    fprintf(tmp, "%s %s %s %d %d %d\n", 
                            livre.code, 
                            livre.titre, 
                            livre.auteur,
                            livre.annee, 
                            livre.nbExemplaires, 
                            livre.nbExemplairesDisponibles
                        );
                        state = 1;
                    }
        }else{
            fprintf(tmp, "%s %s %s %d %d %d\n", 
               livre.code, 
               livre.titre, 
               livre.auteur,
               livre.annee, 
               livre.nbExemplaires, 
               livre.nbExemplairesDisponibles);
        }
    }
    fclose(fichier);
    fclose(tmp);
    remove("livres.txt");
    rename("tmp.txt", "livres.txt"); 

    // etudiant emprunt section
    // etudiant emprunt section
    Etudiant etd = {0};  // Initialize all fields to zero

    // First check if file exists
    FILE *etd_emprunt = fopen("emprunts.txt", "r");
    if (!etd_emprunt) {
        // File doesn't exist, create a new empty one
        etd_emprunt = fopen("emprunts.txt", "w");
        if (!etd_emprunt) return 1;
        fclose(etd_emprunt);
        // Reopen in read mode
        etd_emprunt = fopen("emprunts.txt", "r");
        if (!etd_emprunt) return 1;
    }

    FILE *etd_emprunt_tmp = fopen("tmp_emprunt.txt", "w");
    if (!etd_emprunt || !etd_emprunt_tmp) {
        if (etd_emprunt) fclose(etd_emprunt);
        if (etd_emprunt_tmp) fclose(etd_emprunt_tmp);
        return 1;
    }

    int is_present = 0;
    while(fscanf(etd_emprunt, "%s %s %s %s %s %s %s %s %s %s %s %s %s", 
        etd.prenom, 
        etd.nom, 
        etd.CNIE,
        etd.emtprunts[0], 
        etd.emtprunts[1], 
        etd.emtprunts[2],
        etd.emtprunts[3], 
        etd.emtprunts[4], 
        etd.emtprunts[5],
        etd.emtprunts[6],
        etd.emtprunts[7], 
        etd.emtprunts[8], 
        etd.emtprunts[9]) == 13) {
        
        if(strcmp(etd.CNIE, etudiant->CNIE) == 0){
            is_present = 1;
            int added = 0;
            for(int i = 0; i < 10; i++){
                if(strcmp(etd.emtprunts[i], "") == 0){
                    strcpy(etd.emtprunts[i], codeLivre);
                    added = 1;
                    break;
                }
            }
            if (!added) {
                printf("Erreur: l'etudiant a emprunté le maximum de livres possible \n");
                // Handle error case here
            }
        }
        
        fprintf(etd_emprunt_tmp, "%s %s %s %s %s %s %s %s %s %s %s %s %s\n", 
                etd.prenom, 
                etd.nom, 
                etd.CNIE,
                etd.emtprunts[0], 
                etd.emtprunts[1], 
                etd.emtprunts[2],
                etd.emtprunts[3], 
                etd.emtprunts[4], 
                etd.emtprunts[5],
                etd.emtprunts[6],
                etd.emtprunts[7], 
                etd.emtprunts[8], 
                etd.emtprunts[9]);
    }

    fclose(etd_emprunt);
    fclose(etd_emprunt_tmp);

    if(remove("emprunts.txt") != 0) {
        perror("Error removing file");
        return 1;
    }

    if(rename("tmp_emprunt.txt", "emprunts.txt") != 0) {
        perror("Error renaming file");
        return 1;
    }

    if(!is_present){
        FILE *etd_emprunt_final = fopen("emprunts.txt", "a");
        if (!etd_emprunt_final) return 1;
        
        fprintf(etd_emprunt_final, "%s %s %s %s %s %s %s %s %s %s %s %s %s\n", 
                etudiant->prenom, 
                etudiant->nom, 
                etudiant->CNIE,
                codeLivre,
                "", "", "",
                "", "", "",
                "", "", "");
        
        fclose(etd_emprunt_final);
    }
        // Logique de retour    
        return state;
    }

int rendreLivre(const char* filename, const char* codeLivre) {
    // Logique de retour
    return 1;
}