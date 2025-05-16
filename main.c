#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "storagemanagement.h"
#include "structs.h"
#define MAX_SAISIE 100

// Déclaration des tableaux globaux pour les livres et étudiants
Livre livres[MAX_LIVRES];
Etudiant etudiants[MAX_ETUDIANTS];
int total_livres = 0;        // Nombre total de livres dans le système
int total_etudiants = 0;     // Nombre total d'étudiants dans le système
int start_index = 0;         // Index de départ pour le défilement des listes

// Prototypes des fonctions
void afficherMenuPrincipal();
void gererSaisieLivre();
void gererSuppressionLivre();
void afficherListeLivres();
void afficherListeEtudiant();
void emprunterLivreGUI();
void retournerLivreGUI();
void rechercherLivreGUI();

/**
 * Fonction principale du programme
 * Initialise l'interface ncurses et gère le menu principal
 * @return 0 si le programme se termine normalement
 */
int main() {
    // Initialisation de ncurses
    initscr();              // Initialise l'écran
    cbreak();               // Désactive la mise en mémoire tampon de ligne
    noecho();               // Désactive l'affichage des caractères saisis
    keypad(stdscr, TRUE);   // Active les touches spéciales du clavier
    scrollok(stdscr, TRUE); // Autorise le défilement de l'écran
    curs_set(0);            // Cache le curseur
    start_color();          // Active les couleurs

    // Définition des paires de couleurs
    init_pair(1, COLOR_WHITE, COLOR_BLUE);  // Texte blanc sur fond bleu
    init_pair(2, COLOR_BLACK, COLOR_WHITE); // Texte noir sur fond blanc

    int choix;
    do {
        afficherMenuPrincipal();
        choix = getch();    // Récupère le choix de l'utilisateur
        
        switch(choix) {
            case '1':
                gererSaisieLivre();         // Ajouter un livre
                break;
            case '2':
                afficherListeLivres();      // Afficher la liste des livres
                break;
            case '3':
                gererSuppressionLivre();    // Supprime un livre
                break;
            case '4':
                emprunterLivreGUI();       // Emprunter un livre
                break;
            case '5':
                retournerLivreGUI();       // Retourner un livre
                break;
            case '6':
                afficherListeEtudiant();   // Afficher la liste des étudiants
                break;
            case '7':
                rechercherLivreGUI();      // Rechercher un livre
                break;
            case 'q':
            case 'Q':
                break;                     // Quitter l'application
            default:
                mvprintw(23, 2, "Choix invalide !");
                refresh();
        }
    } while(choix != 'q' || choix!='Q');

    endwin();  // Restaure les paramètres du terminal
    return 0;
}

/**
 * Affiche le menu principal de l'application
 */
void afficherMenuPrincipal() {
    clear();
    attron(COLOR_PAIR(1));
    mvprintw(2, 35, "GESTION BIBLIOTHEQUE");
    attroff(COLOR_PAIR(1));

    // Affichage des options du menu
    mvprintw(5, 30, "1. Ajouter un livre");
    mvprintw(6, 30, "2. Liste des livres");
    mvprintw(7, 30, "3. Emprunter un livre");
    mvprintw(8, 30, "4. Retourner un livre");
    mvprintw(9, 30, "5. Liste des etudiants");
    mvprintw(10, 30, "6. rechercher un livre");
    mvprintw(11, 30, "Q. Quitter");
    
    attron(COLOR_PAIR(2));
    mvprintw(23, 2, "Votre choix : ");
    refresh();
}

/**
 * Gère la saisie d'un nouveau livre
 */
void gererSaisieLivre() {
    clear();
    Livre nouveau;
    
    // Configuration de l'entrée
    keypad(stdscr, true);
    echo();
    curs_set(1);
    
    // Saisie des informations du livre
    mvprintw(2, 30, "NOUVEAU LIVRE (entrer pour sortir)");
    mvprintw(5, 10, "Code : ");
    getnstr(nouveau.code, 9);
    if (nouveau.code[0]==0) return;  // Sortie si champ vide

    mvprintw(6, 10, "Titre : ");
    getnstr(nouveau.titre, 49);
    if (nouveau.titre[0]==0) return;
    
    mvprintw(7, 10, "Auteur : ");
    getnstr(nouveau.auteur, 49);
    if (nouveau.auteur[0]==0) return;
    
    mvprintw(8, 10, "Annee : ");
    scanw("%d", &nouveau.annee);
    if (nouveau.annee==0) return;
    
    mvprintw(9, 10, "Exemplaires : ");
    scanw("%d", &nouveau.nbExemplaires);
    if (nouveau.nbExemplaires==0) return;
    
    nouveau.nbExemplairesDisponibles = nouveau.nbExemplaires;
    
    // Sauvegarde du livre
    if(sauvegarderLivre("livres.txt", &nouveau)) {
        mvprintw(23, 2, "Livre enregistre avec succes !");
    }else {
        mvprintw(23, 2, "Erreur lors de l'enregistrement !");
    }
    
    // Restauration des paramètres d'affichage
    noecho();
    curs_set(0);
    getch();  // Attente d'une saisie utilisateur
}

/**
 * Affiche la liste des livres avec défilement
 */
void afficherListeLivres() {
    int ch;
    chargerLivres("livres.txt", livres, &total_livres);
    
    do {
        clear();
        mvprintw(2, 35, "LISTE DES LIVRES (%d/%d)", start_index + 1, total_livres);
        
        // Affiche une page de livres (adaptée à la taille de l'écran)
        int max_lines = LINES - 6;
        for(int i = 0; i < max_lines && (i + start_index) < total_livres; i++) {
            Livre *l = &livres[i + start_index];
            mvprintw(5 + i, 3, "%s - %s (%s) | Disp: %d/%d",
                    l->code, l->titre, l->auteur, 
                    l->nbExemplairesDisponibles, l->nbExemplaires);
        }

        // Instructions pour l'utilisateur
        mvprintw(LINES - 2, 2, "↑/↓: Defilement | R: Retour");
        refresh();
        
        // Gestion des touches de défilement
        ch = getch();
        switch(ch) {
            case KEY_DOWN:
                if(start_index < total_livres - 1) start_index++;
                break;
            case KEY_UP:
                if(start_index > 0) start_index--;
                break;
            case KEY_NPAGE: // Page suivante
                start_index += max_lines;
                if(start_index >= total_livres) start_index = total_livres - 1;
                break;
            case KEY_PPAGE: // Page précédente
                start_index -= max_lines;
                if(start_index < 0) start_index = 0;
                break;
        }
    } while(ch != 'R' && ch != 'r');
}

/**
 * Interface pour emprunter un livre
 */
void emprunterLivreGUI(){
    clear();
    char codeLivre[MAX_SAISIE];
    Etudiant etd;
    echo();
    curs_set(1);
    
    // Saisie des informations
    mvprintw(2, 30, "EMPRUNT DE LIVRE ou entrer pour quitter ce menu");
    mvprintw(5, 10, "Code du livre : ");
    getnstr(codeLivre, 9);
    if (codeLivre[0]==0) return;

    mvprintw(7, 10, "CNIE :");
    getnstr(etd.CNIE, 14);
    if (etd.CNIE[0]==0) return;

    mvprintw(9, 10, "Nom : ");
    getnstr(etd.nom, 30);
    if (etd.nom[0]==0) return;

    mvprintw(11, 10, "Prenom : ");
    getnstr(etd.prenom, 30);
    if (etd.prenom[0]==0) return;

    // Tentative d'emprunt
    int etat_emprunt = emprunterLivre(&etd, codeLivre);

    // Affichage du résultat
    if(etat_emprunt == 0){
        mvprintw(23, 4, "Emprunt effectue avec succes !");
    }else if(etat_emprunt == 1){
        mvprintw(23, 4, "ther is no book with this code");
    }else if(etat_emprunt == 2){
        mvprintw(23, 4, "no copies available");
    }else{
        mvprintw(23, 4, "Erreur lors de l'emprunt !");
    }
    
    noecho();
    curs_set(0);
    getch();
}

/**
 * Interface pour retourner un livre
 */
void retournerLivreGUI(){
    clear();
    char codeLivre[MAX_SAISIE];
    Etudiant etd={0};
    echo();
    curs_set(1);
    
    // Saisie des informations
    mvprintw(2, 30, "RETOUR DE LIVRE");
    mvprintw(5, 10, "Code du livre : ");
    getnstr(codeLivre, 9);

    mvprintw(7, 10, "CNIE :");
    getnstr(etd.CNIE, 14);
    
    // Tentative de retour
    if(!rendreLivre(&etd, codeLivre)){
        mvprintw(23, 4, "Retour effectue avec succes !");
    }else{
        mvprintw(23, 4, "Erreur lors du retour !");
    }
    
    noecho();
    curs_set(0);
    getch();
}

/**
 * Affiche la liste des étudiants avec leurs emprunts
 */
void afficherListeEtudiant() {
    int ch;
    chargerEtudiant("emprunts.txt", etudiants, &total_etudiants);
    
    do {
        clear();
        mvprintw(2, 35, "LISTE DES ETUDIANT (%d/%d)", start_index + 1, total_etudiants);
        
        int max_lines = LINES - 6;
        for(int i = 0; i < max_lines && (i + start_index) < total_etudiants; i++) {
            Etudiant *l = &etudiants[i + start_index];
            
            // Compte les livres empruntés
            int books_count = 0;
            for (int j = 0; j < 10; j++) {
                if (l->emprunts[j] != NULL) books_count++;
            }
            
            // Affichage selon le nombre d'emprunts
            if (books_count == 0) {
                mvprintw(5 + i, 3, "%s - %s %s | Emprunts: Aucun",
                        l->CNIE, l->nom, l->prenom);
            } else {
                mvprintw(5 + i, 3, "%s - %s %s | Emprunts: %s (+%d restant(s) ouvert(s))",
                        l->CNIE, l->nom, l->prenom, l->emprunts[0], books_count-1);
            }
        }
        
        mvprintw(LINES - 2, 2, "↑/↓: Defilement | R: Retour");
        refresh();
        
        ch = getch();
        switch(ch) {
            case KEY_DOWN:
                if(start_index < total_livres - 1) start_index++;
                break;
            case KEY_UP:
                if(start_index > 0) start_index--;
                break;
            case KEY_NPAGE:
                start_index += max_lines;
                if(start_index >= total_livres) start_index = total_livres - 1;
                break;
            case KEY_PPAGE:
                start_index -= max_lines;
                if(start_index < 0) start_index = 0;
                break;
        }
    } while(ch != 'R' && ch != 'r');
}

/**
 * Interface de recherche de livres
 */
void rechercherLivreGUI() {
    list_simple *livres = NULL;
    int ch, nbTrouves = 0;
    int start_index = 0;
    char critere[MAX_SAISIE];

    clear();
    echo();
    curs_set(1);

    // Menu de recherche
    mvprintw(2, 30, "RECHERCHER UN LIVRE");
    mvprintw(4, 10, "1. Titre");
    mvprintw(5, 10, "2. Auteur");
    mvprintw(6, 10, "3. Annee");
    mvprintw(7, 10, "0. Code");
    mvprintw(8, 10, "4. Retour au menu principal");
    mvprintw(10, 10, "Choix : ");
    refresh();

    int choix = getch() - '0';
    if (choix < 0 || choix > 4) return;
    if (choix == 4) return;

    clear();
    mvprintw(2, 30, "RECHERCHER UN LIVRE (Entrer pour quitter)");

    switch (choix) {
        case 1:
            mvprintw(4, 30, "Titre : ");
            getnstr(critere, 50);
            livres = rechercherLivres("livres.txt", critere, 1, &nbTrouves);
            break;
        case 2:
            mvprintw(4, 30, "Auteur : ");
            getnstr(critere, 50);
            livres = rechercherLivres("livres.txt", critere, 2, &nbTrouves);
            break;
        case 3:
            mvprintw(4, 30, "Annee : ");
            getnstr(critere, 4);
            livres = rechercherLivres("livres.txt", critere, 3, &nbTrouves);
            break;
        case 0:
            mvprintw(4, 30, "Code : ");
            getnstr(critere, 10);
            livres = rechercherLivres("livres.txt", critere, 0, &nbTrouves);
            break;
    }

    if (livres == NULL || nbTrouves <= 0) {
        mvprintw(6, 30, "Aucun livre trouvé !");
        getch();
    } else {
        // Transformer liste chaînée en tableau pour faciliter le défilement
        Livre **tableau = (Livre**)malloc(nbTrouves * sizeof(Livre*));
        list_simple *tmp = livres;
        for (int i = 0; i < nbTrouves && tmp != NULL; i++, tmp = tmp->suivant)
            tableau[i] = tmp->liver;

        do {
            clear();
            mvprintw(2, 30, "LISTE DES LIVRES (%d/%d)", start_index + 1, nbTrouves);

            int max_lines = LINES - 6;
            for (int i = 0; i < max_lines && (i + start_index) < nbTrouves; i++) {
                Livre *l = tableau[i + start_index];
                mvprintw(5 + i, 3, "%s - %s (%s) | Annee: %d | Disp: %d/%d",
                         l->code, l->titre, l->auteur, l->annee,
                         l->nbExemplairesDisponibles, l->nbExemplaires);
            }

            mvprintw(LINES - 2, 2, "↑/↓: Defilement | PgUp/PgDn: Page | R: Retour");
            refresh();

            ch = getch();
            switch (ch) {
                case KEY_DOWN:
                    if (start_index < nbTrouves - 1) start_index++;
                    break;
                case KEY_UP:
                    if (start_index > 0) start_index--;
                    break;
                case KEY_NPAGE:
                    start_index += max_lines;
                    if (start_index >= nbTrouves) start_index = nbTrouves - 1;
                    break;
                case KEY_PPAGE:
                    start_index -= max_lines;
                    if (start_index < 0) start_index = 0;
                    break;
            }

        } while (ch != 'r' && ch != 'R');

        free(tableau); // Libération du tableau temporaire
    }

    if (livres) libererListe(livres); // Libération de la liste
    noecho();
    curs_set(0);
}
/**
 * interface pour la suppression d'un livre
 */
void gererSuppressionLivre(){
    clear();
    Livre supp;

    keypad(stdscr, true);
    echo();
    curs_set(1);

    mvprintw(2, 30, "Supression de livre (entrer pour sortir)");
    mvprintw(5, 10, "Code: ");
    getnstr(supp.code, 9);
    if (supp.code[0]==0) return;

    if (supprimerLivre(supp.code)){
        mvprintw(23, 2, "Livre supprimer avec succes !");
    }else {
        mvprintw(23, 2, "Erreur lors de la suppression !");
    }

    noecho();
    curs_set(0);
    getch();
}