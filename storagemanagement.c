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

int chargerEtudiant(const char* filename, Etudiant *etudiants, int *total) {
    *total = 0;
    FILE *fichier = fopen(filename, "r");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        return 0;
    }

    char line[512];
    while (*total < MAX_ETUDIANTS && fgets(line, sizeof(line), fichier)) {
        // Initialize all emprunts to empty strings
        for (int i = 0; i < 10; i++) {
            etudiants[*total].emprunts[i][0] = '\0';
        }

        // Use temporary variables for safer parsing
        char prenom[50], nom[50], cnie[20];
        int count = sscanf(line, "%49s %49s %19s", prenom, nom, cnie);
        
        if (count < 3) continue;  // Skip invalid lines

        // Copy the basic info
        strncpy(etudiants[*total].prenom, prenom, sizeof(etudiants[*total].prenom) - 1);
        strncpy(etudiants[*total].nom, nom, sizeof(etudiants[*total].nom) - 1);
        strncpy(etudiants[*total].CNIE, cnie, sizeof(etudiants[*total].CNIE) - 1);

        // Parse book loans
        char *ptr = line;
        // Skip the first three fields
        for (int i = 0; i < 3; i++) {
            ptr += strcspn(ptr, " \t");  // Skip current field
            ptr += strspn(ptr, " \t");   // Skip whitespace
        }

        // Parse up to 10 book codes
        for (int i = 0; i < 10 && *ptr != '\0' && *ptr != '\n'; i++) {
            int len = strcspn(ptr, " \t\n");
            if (len > 0) {
                strncpy(etudiants[*total].emprunts[i], ptr, len);
                etudiants[*total].emprunts[i][len] = '\0';
                ptr += len;
                ptr += strspn(ptr, " \t");  // Skip whitespace
            }
        }

        (*total)++;
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


Livre* rechercherLivres(const char* filename, const char* critere, int type, int* nbTrouves) {
    FILE *fichier = fopen(filename, "r");
    if (!fichier) {
        *nbTrouves = -1;  // Erreur ouverture fichier
        return NULL;
    }

    Livre *resultats = (Livre*)malloc(sizeof(Livre) * MAX_LIVRES);
    if (!resultats) {
        fclose(fichier);
        *nbTrouves = -2;  // Erreur allocation
        return NULL;
    }

    Livre temp;
    int count = 0;

    while (fscanf(fichier, "%s %s %s %d %d %d",
                  temp.code, temp.titre, temp.auteur,
                  &temp.annee, &temp.nbExemplaires, &temp.nbExemplairesDisponibles) == 6) {

        int match = 0;
        switch (type) {
            case 0: match = (strcmp(temp.code, critere) == 0); break;
            case 1: match = (strcmp(temp.titre, critere) == 0); break;
            case 2: match = (strcmp(temp.auteur, critere) == 0); break;
            case 3: match = (temp.annee == atoi(critere)); break;
            default: match = 0;
        }

        if (match) {
            resultats[count++] = temp;
            if (count >= MAX_LIVRES) break;
        }
    }
    fclose(fichier);
    *nbTrouves = count;

    // Si aucun livre trouvé, libérer la mémoire et retourner NULL
    if (count == 0) {
        free(resultats);
        return NULL;
    }
    return resultats;
}

int emprunterLivre(Etudiant* etudiant, const char* codeLivre) {
    // Book borrowing section - Fixed book existence check
    FILE *fichier = fopen("livres.txt", "r");
    if (!fichier) return 1;
    
    FILE *tmp = fopen("tmp.txt", "w");
    if (!tmp) { fclose(fichier); return 1; }
    
    Livre livre;
    int book_exists = 0;
    while (fscanf(fichier, "%s %s %s %d %d %d", 
           livre.code, livre.titre, livre.auteur,
           &livre.annee, &livre.nbExemplaires, &livre.nbExemplairesDisponibles) == 6) {
        
        if (!strcmp(livre.code, codeLivre)) {
            book_exists = 1;
            if (livre.nbExemplairesDisponibles > 0) {
                livre.nbExemplairesDisponibles--;
            } else {
                return 2;  // Book exists but no copies available
            }
        }
        
        fprintf(tmp, "%s %s %s %d %d %d\n", 
               livre.code, livre.titre, livre.auteur,
               livre.annee, livre.nbExemplaires, livre.nbExemplairesDisponibles);
    }
    
    fclose(fichier);
    fclose(tmp);
    remove("livres.txt");
    rename("tmp.txt", "livres.txt");

    if (!book_exists) return 1;  // Book doesn't exist at all

    // Student borrowing section - Fixed data persistence
    FILE *etd_emprunt = fopen("emprunts.txt", "r");
    FILE *tmp_emprunt = fopen("tmp_emprunt.txt", "w");
    if (!tmp_emprunt) { 
        if (etd_emprunt) fclose(etd_emprunt); 
        return -1; 
    }

    Etudiant etd = {0};
    int is_present = 0;
    
    while (etd_emprunt && fscanf(etd_emprunt, "%s %s %s %s %s %s %s %s %s %s %s %s %s", 
           etd.prenom, etd.nom, etd.CNIE, etd.emprunts[0], etd.emprunts[1],
           etd.emprunts[2], etd.emprunts[3], etd.emprunts[4], etd.emprunts[5],
           etd.emprunts[6], etd.emprunts[7], etd.emprunts[8], etd.emprunts[9]) == 13) {
        
        if (!strcmp(etd.CNIE, etudiant->CNIE)) {
            is_present = 1;
            int added = 0;
            for (int i = 0; i < 10 && !added; i++) {
                if (strcmp(etd.emprunts[i], "NULL") == 0) {
                    strcpy(etd.emprunts[i], codeLivre);
                    added = 1;
                }
            }
            if (!added) {
                printf("Erreur: Maximum de livres empruntés\n");
            }
        }

        // Write all fields with NULL placeholder
        fprintf(tmp_emprunt, "%s %s %s %s %s %s %s %s %s %s %s %s %s\n", 
                etd.prenom, etd.nom, etd.CNIE,
                etd.emprunts[0], etd.emprunts[1], etd.emprunts[2],
                etd.emprunts[3], etd.emprunts[4], etd.emprunts[5],
                etd.emprunts[6], etd.emprunts[7], etd.emprunts[8],
                etd.emprunts[9]);
    }
    
    if (etd_emprunt) fclose(etd_emprunt);
    fclose(tmp_emprunt);
    
    remove("emprunts.txt");
    rename("tmp_emprunt.txt", "emprunts.txt");

    if (!is_present) {
        FILE *f = fopen("emprunts.txt", "a");
        if (f) {
            fprintf(f, "%s %s %s %s NULL NULL NULL NULL NULL NULL NULL NULL NULL\n", 
                    etudiant->prenom, etudiant->nom, 
                    etudiant->CNIE, codeLivre);
            fclose(f);
        }
    }
    
    return 1; //success
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
                    etd.emprunts[0], etd.emprunts[1], etd.emprunts[2],
                    etd.emprunts[3], etd.emprunts[4], etd.emprunts[5],
                    etd.emprunts[6], etd.emprunts[7], etd.emprunts[8],
                    etd.emprunts[9]) == 13) {
        int num_emprunt = 0;
        if (strcmp(etd.CNIE, etudiant->CNIE) == 0) {
            for (int i = 0; i < 10; i++) {
                if (strcmp(etd.emprunts[i], codeLivre) == 0) {
                    strcpy(etd.emprunts[i], "NULL");
                    num_emprunt += 1;
                    break;
                }else if(strcmp(etd.emprunts[i],"NULL")){
                    num_emprunt += 1;
                }
            }
        }
        if(num_emprunt <=10){
            fprintf(etd_emprunt_tmp, "%s %s %s %s %s %s %s %s %s %s %s %s %s\n",
                etd.prenom, etd.nom, etd.CNIE,
                etd.emprunts[0], etd.emprunts[1], etd.emprunts[2],
                etd.emprunts[3], etd.emprunts[4], etd.emprunts[5],
                etd.emprunts[6], etd.emprunts[7], etd.emprunts[8],
                etd.emprunts[9]);
            }
    }

    fclose(fichier);
    fclose(etd_emprunt_tmp);
    remove("emprunts.txt");
    rename("tmp_emprunt.txt", "emprunts.txt");

    return state;
}
