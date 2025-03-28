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
    Etudiant etd;
    FILE *etd_emprunt = fopen("emprunts.txt", "a+");
    FILE *etd_emprunt_tmp = fopen("tmp_emprunt.txt", "w");
    if (!etd_emprunt) return 1;
    int is_presnt = 0;
    while(!feof(etd_emprunt)){
        fscanf(etd_emprunt,"%s %s %s %s %s %s %s %s %s %s\n", 
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
        
        if(strcmp(etd.CNIE, etudiant->CNIE) == 0){
            for(int i =0; i < 10;i++){
                if(strcmp(etd.emtprunts[i], "") == 0){
                    strcat(etd.emtprunts[i], codeLivre);;
                    break;
                }
            
            fprintf(etd_emprunt_tmp, "%s %s %s %s %s %s %s %s %s %s\n", 
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
            is_presnt = 1;
        }else{
            fprintf(etd_emprunt_tmp, "%s %s %s %s %s %s %s %s %s %s\n", 
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
    }
    fclose(etd_emprunt);
    fclose(etd_emprunt_tmp);
    remove("emprunts.txt");
    rename("tmp_emprunt.txt", "emprunts.txt");
    FILE *etd_emprunt_final = fopen("emprunts.txt", "a");
    if (!etd_emprunt_final) return 1;
    if(!is_presnt){
        fprintf(etd_emprunt_final, "%s %s %s %s %s %s %s %s %s %s\n", 
                etudiant->prenom, 
                etudiant->nom, 
                etudiant->CNIE,
                codeLivre, 
                etudiant->emtprunts[1], 
                etudiant->emtprunts[2],
                etudiant->emtprunts[3], 
                etudiant->emtprunts[4], 
                etudiant->emtprunts[5],
                etudiant->emtprunts[6],
                etudiant->emtprunts[7], 
                etudiant->emtprunts[8], 
                etudiant->emtprunts[9]);
    }

    return state;
}

int rendreLivre(const char* filename, const char* codeLivre) {
    // Logique de retour
    return 1;
}