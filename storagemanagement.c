#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "storagemanagement.h"
#include "structs.h"

/**
 * Charge les livres depuis un fichier
 * @param filename Nom du fichier à charger
 * @param livres Tableau de livres où stocker les données
 * @param total Pointeur pour retourner le nombre total de livres chargés
 * @return 1 si succès, 0 si échec
 */
int chargerLivres(const char* filename, Livre *livres, int *total) {
    *total = 0;
    FILE *fichier = fopen(filename, "r");
    if (!fichier) return 0;

    char line[256];
    while(fgets(line, sizeof(line), fichier) && *total < MAX_LIVRES) {
        char titre_temp[50], auteur_temp[50];
        if(sscanf(line, "%s %s %s %d %d %d", 
               livres[*total].code, 
               titre_temp, 
               auteur_temp,
               &livres[*total].annee, 
               &livres[*total].nbExemplaires, 
               &livres[*total].nbExemplairesDisponibles) == 6) {
            
            // Convert asterisks back to spaces
            strcpy(livres[*total].titre, titre_temp);
            strcpy(livres[*total].auteur, auteur_temp);
            for (int i = 0; livres[*total].titre[i]; i++) 
                if (livres[*total].titre[i] == '*') livres[*total].titre[i] = ' ';
            for (int i = 0; livres[*total].auteur[i]; i++) 
                if (livres[*total].auteur[i] == '*') livres[*total].auteur[i] = ' ';
            
            (*total)++;
        }
    }
    fclose(fichier);
    return 1;
}

/**
 * Charge les étudiants depuis un fichier
 * @param filename Nom du fichier à charger
 * @param etudiants Tableau d'étudiants où stocker les données
 * @param total Pointeur pour retourner le nombre total d'étudiants chargés
 * @return 1 si succès, 0 si échec
 */
int chargerEtudiant(const char* filename, Etudiant *etudiants, int *total) {
    *total = 0;
    FILE *fichier = fopen(filename, "r");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        return 0;
    }

    char line[512];
    while (*total < MAX_ETUDIANTS && fgets(line, sizeof(line), fichier)) {
        // Initialise tous les emprunts à des chaînes vides
        for (int i = 0; i < 10; i++) {
            etudiants[*total].emprunts[i][0] = '\0';
        }

        // Utilise des variables temporaires pour un parsing plus sûr
        char prenom[50], nom[50], cnie[20];
        int count = sscanf(line, "%49s %49s %19s", prenom, nom, cnie);
        
        if (count < 3) continue;  // Ignore les lignes invalides

        // Copie les informations de base
        strncpy(etudiants[*total].prenom, prenom, sizeof(etudiants[*total].prenom) - 1);
        strncpy(etudiants[*total].nom, nom, sizeof(etudiants[*total].nom) - 1);
        strncpy(etudiants[*total].CNIE, cnie, sizeof(etudiants[*total].CNIE) - 1);

        // Parse les emprunts de livres
        char *ptr = line;
        // Saute les trois premiers champs
        for (int i = 0; i < 3; i++) {
            ptr += strcspn(ptr, " \t");  // Saute le champ courant
            ptr += strspn(ptr, " \t");   // Saute les espaces
        }

        // Parse jusqu'à 10 codes de livre
        for (int i = 0; i < 10 && *ptr != '\0' && *ptr != '\n'; i++) {
            int len = strcspn(ptr, " \t\n");
            if (len > 0) {
                strncpy(etudiants[*total].emprunts[i], ptr, len);
                etudiants[*total].emprunts[i][len] = '\0';
                ptr += len;
                ptr += strspn(ptr, " \t");  // Saute les espaces
            }
        }

        (*total)++;
    }

    fclose(fichier);
    return 1;
}

/**
 * Sauvegarde un livre dans un fichier
 * @param filename Nom du fichier
 * @param l Pointeur vers le livre à sauvegarder
 * @return 1 si succès, 0 si échec
 */
int sauvegarderLivre(const char* filename, Livre *l) {
    FILE *fichier = fopen(filename, "a");
    if (!fichier) {
        return 0;
    }
    
    // Replace spaces with asterisks in title and author
    char titre_temp[50], auteur_temp[50];
    strcpy(titre_temp, l->titre);
    strcpy(auteur_temp, l->auteur);
    
    for (int i = 0; titre_temp[i]; i++) if (titre_temp[i] == ' ') titre_temp[i] = '*';
    for (int i = 0; auteur_temp[i]; i++) if (auteur_temp[i] == ' ') auteur_temp[i] = '*';
    
    fprintf(fichier, "%s %s %s %d %d %d\n", 
           l->code, titre_temp, auteur_temp, 
           l->annee, l->nbExemplaires, 
           l->nbExemplairesDisponibles);
    
    fclose(fichier);
    return 1;
}

/**
 * Recherche des livres selon un critère
 * @param filename Nom du fichier à chercher
 * @param critere Valeur à rechercher
 * @param type Type de recherche (0=code, 1=titre, 2=auteur, 3=année)
 * @param nbTrouves Pointeur pour retourner le nombre de résultats
 * @return Tableau de livres trouvés ou NULL
 */
list_simple* rechercherLivres(const char* filename, const char* critere, int type, int* nbTrouves) {
    FILE *fichier = fopen(filename, "r");
    if (!fichier) {
        *nbTrouves = -1;
        return NULL;
    }

    list_simple *premier_livre = NULL, *courant = NULL;
    *nbTrouves = 0;

    Livre temp;

    while (fscanf(fichier, "%s %s %s %d %d %d",
                  temp.code, temp.titre, temp.auteur,
                  &temp.annee, &temp.nbExemplaires, &temp.nbExemplairesDisponibles) == 6) {

        // Remplacement des * par des espaces
        for (int i = 0; temp.titre[i]; i++) if (temp.titre[i] == '*') temp.titre[i] = ' ';
        for (int i = 0; temp.auteur[i]; i++) if (temp.auteur[i] == '*') temp.auteur[i] = ' ';

        int match = 0;
        switch (type) {
            case 0: match = (strcmp(temp.code, critere) == 0); break;
            case 1: match = (strcmp(temp.titre, critere) == 0); break;
            case 2: match = (strcmp(temp.auteur, critere) == 0); break;
            case 3: match = (temp.annee == atoi(critere)); break;
        }

        if (match) {
            list_simple *nouveau = (list_simple*)malloc(sizeof(list_simple));
            if (!nouveau) continue;

            nouveau->liver = (Livre*)malloc(sizeof(Livre));
            if (!nouveau->liver) {
                free(nouveau);
                continue;
            }

            *(nouveau->liver) = temp;
            nouveau->suivant = NULL;

            if (premier_livre == NULL) {
                premier_livre = nouveau;
                courant = nouveau;
            } else {
                courant->suivant = nouveau;
                courant = nouveau;
            }

            (*nbTrouves)++;
        }
    }

    fclose(fichier);

    if (*nbTrouves == 0 && premier_livre != NULL) {
        // Aucun livre trouvé mais liste allouée par erreur
        // (sécurité redondante ici)
        list_simple *tmp;
        while (premier_livre) {
            tmp = premier_livre;
            premier_livre = premier_livre->suivant;
            free(tmp->liver);
            free(tmp);
        }
        return NULL;
    }

    return premier_livre;
}

/**
 * Libère la mémoire occupée par une liste simplement chaînée de livres.
 *
 * @param tete Pointeur vers la tête de la liste à libérer.
 */
void libererListe(list_simple* tete) {
    list_simple* tmp;
    while (tete) {
        tmp = tete;
        tete = tete->suivant;
        free(tmp->liver);
        free(tmp);
    }
}

/**
 * Permet à un étudiant d'emprunter un livre
 * @param etudiant Pointeur vers l'étudiant
 * @param codeLivre Code du livre à emprunter
 * @return 1 si succès, 2 si pas d'exemplaires disponibles, -1 si erreur
 */
int emprunterLivre(Etudiant* etudiant, const char* codeLivre) {
    FILE *fichier = fopen("livres.txt", "r");
    if (!fichier) return 1;
    
    FILE *tmp = fopen("tmp.txt", "w");
    if (!tmp) { fclose(fichier); return 1; }
    
    Livre livre;
    char titre_temp[50], auteur_temp[50];
    int book_exists = 0;
    while (fscanf(fichier, "%s %s %s %d %d %d", 
           livre.code, titre_temp, auteur_temp,
           &livre.annee, &livre.nbExemplaires, &livre.nbExemplairesDisponibles) == 6) {
        
        // Convert back to original format for comparison
        strcpy(livre.titre, titre_temp);
        strcpy(livre.auteur, auteur_temp);
        
        if (!strcmp(livre.code, codeLivre)) {
            book_exists = 1;
            if (livre.nbExemplairesDisponibles > 0) {
                livre.nbExemplairesDisponibles--;
            } else {
                return 2;
            }
        }
        
        fprintf(tmp, "%s %s %s %d %d %d\n", 
               livre.code, titre_temp, auteur_temp,
               livre.annee, livre.nbExemplaires, livre.nbExemplairesDisponibles);
    }
    
    fclose(fichier);
    fclose(tmp);
    remove("livres.txt");
    rename("tmp.txt", "livres.txt");

    if (!book_exists) return 1;

    // Gestion de l'emprunt par l'étudiant
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

        // Écrit tous les champs avec le marqueur NULL
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
    
    return 1; //succès
}

/**
 * Permet à un étudiant de rendre un livre
 * @param etudiant Pointeur vers l'étudiant
 * @param codeLivre Code du livre à rendre
 * @return 0 si succès, 1 si erreur
 */
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
    char titre_temp[50], auteur_temp[50];
    int state = 0;
    while (fscanf(livres, "%s %s %s %d %d %d", 
            livre.code, titre_temp, auteur_temp,
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
                livre.code, titre_temp, auteur_temp,
                livre.annee, livre.nbExemplaires, 
                livre.nbExemplairesDisponibles);
    }
    fclose(livres);
    fclose(tmp);
    remove("livres.txt");
    rename("tmp.txt", "livres.txt");

    // Met à jour la liste des emprunts de l'étudiant
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
                    strcpy(etd.emprunts[i], "NULL");  // Libère l'emprunt
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