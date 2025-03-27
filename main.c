#include <ncurses.h>
#include "storagemanagement.h"
#include "structs.h"

#define MAX_SAISIE 100

WINDOW *creerFenetre(int hauteur, int largeur, int y, int x);
void afficherMenuPrincipal();
void gererSaisieLivre();
void afficherListeLivres();

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);

    int choix;
    do {
        afficherMenuPrincipal();
        choix = getch();
        
        switch(choix) {
            case '1':
                gererSaisieLivre();
                break;
            case '2':
                afficherListeLivres();
                break;
            case '3':
                // Emprunter un livre
                break;
            case 'q':
                break;
            default:
                mvprintw(23, 2, "Choix invalide !");
                refresh();
        }
    } while(choix != 'q');

    endwin();
    return 0;
}

void afficherMenuPrincipal() {
    clear();
    attron(COLOR_PAIR(1));
    mvprintw(2, 35, "GESTION BIBLIOTHEQUE");
    attroff(COLOR_PAIR(1));

    mvprintw(5, 30, "1. Ajouter un livre");
    mvprintw(6, 30, "2. Liste des livres");
    mvprintw(7, 30, "3. Emprunter un livre");
    mvprintw(8, 30, "4. Retourner un livre");
    mvprintw(9, 30, "5. Liste des etudiants");
    mvprintw(10, 30, "q. Quitter");
    
    attron(COLOR_PAIR(2));
    mvprintw(23, 2, "Votre choix : ");
    refresh();
}

void gererSaisieLivre() {
    clear();
    Livre nouveau;
    
    echo();
    curs_set(1);
    
    mvprintw(2, 30, "NOUVEAU LIVRE");
    mvprintw(5, 10, "Code : ");
    getnstr(nouveau.code, 9);
    
    mvprintw(6, 10, "Titre : ");
    getnstr(nouveau.titre, 49);
    
    mvprintw(7, 10, "Auteur : ");
    getnstr(nouveau.auteur, 49);
    
    mvprintw(8, 10, "Année : ");
    scanw("%d", &nouveau.annee);
    
    mvprintw(9, 10, "Exemplaires : ");
    scanw("%d", &nouveau.nbExemplaires);
    
    nouveau.nbExemplairesDisponibles = nouveau.nbExemplaires;
    
    if(sauvegarderLivre("livres.txt", &nouveau)) {
        mvprintw(23, 2, "Livre enregistré avec succès !");
    }
    
    noecho();
    curs_set(0);
    getch();
}

void afficherListeLivres() {
    clear();
    mvprintw(2, 35, "LISTE DES LIVRES");
    chargerLivres("livres.txt");
    mvprintw(23, 2, "Appuyez sur une touche pour continuer...");
    refresh();
    getch();
}