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
    // Book borrowing section
    FILE *fichier = fopen("livres.txt", "r");
    if (!fichier) return 1;
    
    FILE *tmp = fopen("tmp.txt", "w");
    if (!tmp) { fclose(fichier); return 1; }
    
    Livre livre;
    int state = 0;
    while (fscanf(fichier, "%s %s %s %d %d %d", 
           livre.code, livre.titre, livre.auteur,
           &livre.annee, &livre.nbExemplaires, &livre.nbExemplairesDisponibles) == 6) {
        
        if (!strcmp(livre.code, codeLivre) && livre.nbExemplairesDisponibles > 0) {
            livre.nbExemplairesDisponibles--;
        } else if (!strcmp(livre.code, codeLivre)) {
            state = 1;
        }
        
        fprintf(tmp, "%s %s %s %d %d %d\n", 
               livre.code, livre.titre, livre.auteur,
               livre.annee, livre.nbExemplaires, livre.nbExemplairesDisponibles);
    }
    fclose(fichier);
    fclose(tmp);
    remove("livres.txt");
    rename("tmp.txt", "livres.txt");

    // Student borrowing section
    FILE *etd_emprunt = fopen("emprunts.txt", "r");
    FILE *tmp_emprunt = fopen("tmp_emprunt.txt", "w");
    if (!tmp_emprunt) { if (etd_emprunt) fclose(etd_emprunt); return 1; }

    Etudiant etd = {0};
    int is_present = 0;
    
    while (etd_emprunt && fscanf(etd_emprunt, "%s %s %s %s %s %s %s %s %s %s %s %s %s", 
           etd.prenom, etd.nom, etd.CNIE, etd.emtprunts[0], etd.emtprunts[1],
           etd.emtprunts[2], etd.emtprunts[3], etd.emtprunts[4], etd.emtprunts[5],
           etd.emtprunts[6], etd.emtprunts[7], etd.emtprunts[8], etd.emtprunts[9]) == 13) {
        
        if (!strcmp(etd.CNIE, etudiant->CNIE)) {
            is_present = 1;
            int added = 0;
            for (int i = 0; i < 10 && !added; i++) {
                if (!strcmp(etd.emtprunts[i], "")) {
                    strcpy(etd.emtprunts[i], codeLivre);
                    added = 1;
                }
            }
            if (!added) printf("Erreur: Maximum de livres empruntés\n");
        }
        
        fprintf(tmp_emprunt, "%s %s %s %s %s %s %s %s %s %s %s %s %s\n", 
                etd.prenom, etd.nom, etd.CNIE, etd.emtprunts[0], etd.emtprunts[1],
                etd.emtprunts[2], etd.emtprunts[3], etd.emtprunts[4], etd.emtprunts[5],
                etd.emtprunts[6], etd.emtprunts[7], etd.emtprunts[8], etd.emtprunts[9]);
    }
    
    if (etd_emprunt) fclose(etd_emprunt);
    fclose(tmp_emprunt);
    
    remove("emprunts.txt");
    rename("tmp_emprunt.txt", "emprunts.txt");

    if (!is_present) {
        FILE *f = fopen("emprunts.txt", "a");
        if (f) {
            fprintf(f, "%s %s %s %s %*s\n", etudiant->prenom, etudiant->nom, 
                    etudiant->CNIE, codeLivre, 9*3, ""); // 9 empty strings (3 chars each)
            fclose(f);
        }
    }
    
    return state;
}

int rendreLivre(Etudiant* etudiant, const char* codeLivre) {
    FILE *livres = fopen("livres.txt", "r");
    FILE *fichier = fopen("emprunts.txt", "r");
    FILE *tmp = fopen("tmp.txt", "w");
    if (!livres || !fichier || !tmp) {
        if (livres) fclose(livres);
        if (fichier) fclose(fichier);
        if (tmp) fclose(tmp);
        return 1;
    }

    Livre livre;
    int state = 0;
    while (fscanf(livres, "%s %s %s %d %d %d", 
            livre.code, livre.titre, livre.auteur,
            &livre.annee, &livre.nbExemplaires, 
            &livre.nbExemplairesDisponibles) == 6) {
        if (strcmp(livre.code, codeLivre) == 0) {
            if (livre.nbExemplairesDisponibles < livre.nbExemplaires) {
                livre.nbExemplairesDisponibles++;
            } else {
                state = 1;
            }
        }
        fprintf(tmp, "%s %s %s %d %d %d\n", 
                livre.code, livre.titre, livre.auteur,
                livre.annee, livre.nbExemplaires, 
                livre.nbExemplairesDisponibles);
    }
    fclose(livres);
    fclose(tmp);
    remove("livres.txt");
    rename("tmp.txt", "livres.txt");

    FILE *etd_emprunt_tmp = fopen("tmp_emprunt.txt", "w");
    Etudiant etd;
    rewind(fichier);
    while (fscanf(fichier, "%s %s %s %s %s %s %s %s %s %s %s %s %s",
                    etd.prenom, etd.nom, etd.CNIE,
                    etd.emtprunts[0], etd.emtprunts[1], etd.emtprunts[2],
                    etd.emtprunts[3], etd.emtprunts[4], etd.emtprunts[5],
                    etd.emtprunts[6], etd.emtprunts[7], etd.emtprunts[8],
                    etd.emtprunts[9]) == 13) {
        if (strcmp(etd.CNIE, etudiant->CNIE) == 0) {
            for (int i = 0; i < 10; i++) {
                if (strcmp(etd.emtprunts[i], codeLivre) == 0) {
                    strcpy(etd.emtprunts[i], "");
                    break;
                }
            }
        }
        fprintf(etd_emprunt_tmp, "%s %s %s %s %s %s %s %s %s %s %s %s %s\n",
                etd.prenom, etd.nom, etd.CNIE,
                etd.emtprunts[0], etd.emtprunts[1], etd.emtprunts[2],
                etd.emtprunts[3], etd.emtprunts[4], etd.emtprunts[5],
                etd.emtprunts[6], etd.emtprunts[7], etd.emtprunts[8],
                etd.emtprunts[9]);
    }

    fclose(fichier);
    fclose(etd_emprunt_tmp);
    remove("emprunts.txt");
    rename("tmp_emprunt.txt", "emprunts.txt");

    return state;
}