#include <ncurses.h>
#include "storagemanagement.h"
#include "structs.h"

#define MAX_SAISIE 100
// hi
Livre livres[MAX_LIVRES];
Etudiant etudiants[MAX_ETUDIANTS];
int total_livres = 0;
int total_etudiants = 0;
int start_index = 0;
WINDOW *creerFenetre(int hauteur, int largeur, int y, int x);
void afficherMenuPrincipal();
void gererSaisieLivre();
void afficherListeLivres();
void afficherListeEtudiant();
void emprunterLivreGUI();
void retournerLivreGUI();

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);
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
                emprunterLivreGUI();
                break;
            case '4':
                // Retourner un livre
                retournerLivreGUI();
                break;
            case '5':
                afficherListeEtudiant();
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
    
    mvprintw(8, 10, "Annee : ");
    scanw("%d", &nouveau.annee);
    
    mvprintw(9, 10, "Exemplaires : ");
    scanw("%d", &nouveau.nbExemplaires);
    
    nouveau.nbExemplairesDisponibles = nouveau.nbExemplaires;
    
    if(sauvegarderLivre("livres.txt", &nouveau)) {
        mvprintw(23, 2, "Livre enregistre avec succes !");
    }else {
        mvprintw(23, 2, "Erreur lors de l'enregistrement !");
    }
    
    noecho();
    curs_set(0);
    getch();
}

void afficherListeLivres() {
    int ch;
    chargerLivres("livres.txt", livres, &total_livres);
    
    do {
        clear();
        mvprintw(2, 35, "LISTE DES LIVRES (%d/%d)", start_index + 1, total_livres);
        
        int max_lines = LINES - 6;
        for(int i = 0; i < max_lines && (i + start_index) < total_livres; i++) {
            Livre *l = &livres[i + start_index];
            mvprintw(5 + i, 3, "%s - %s (%s) | Disp: %d/%d", 
                    l->code, l->titre, l->auteur, 
                    l->nbExemplairesDisponibles, l->nbExemplaires);
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
            case KEY_NPAGE: // Page down
                start_index += max_lines;
                if(start_index >= total_livres) start_index = total_livres - 1;
                break;
            case KEY_PPAGE: // Page up
                start_index -= max_lines;
                if(start_index < 0) start_index = 0;
                break;
        }
    } while(ch != 'R' && ch != 'r');
}

void emprunterLivreGUI(){
    clear();
    char codeLivre[MAX_SAISIE];
    Etudiant etd;
    echo();
    curs_set(1);
    mvprintw(2, 30, "EMPRUNT DE LIVRE");
    mvprintw(5, 10, "Code du livre : ");
    getnstr(codeLivre, 9); // 9 : the first 4 chars and the othor 5 numbers

    mvprintw(7, 10, "CNIE :");
    getnstr(etd.CNIE, 14); // 14 is too much but just in case

    mvprintw(9, 10, "Nom : ");
    getnstr(etd.nom, 30);

    mvprintw(11, 10, "Prenom : ");
    getnstr(etd.prenom, 30);
    
    if(!emprunterLivre(&etd, codeLivre)){ // emprunterLivre returns 0 if the operation is successful
        mvprintw(23, 4, "Emprunt effectue avec succes !");
    }else{
        mvprintw(23, 4, "Erreur lors de l'emprunt !");
    }
    noecho();
    curs_set(0);
    getch();
}

void retournerLivreGUI(){
    clear();
    char codeLivre[MAX_SAISIE];
    Etudiant etd={0};
    echo();
    curs_set(1);
    mvprintw(2, 30, "RETOUR DE LIVRE");
    mvprintw(5, 10, "Code du livre : ");
    getnstr(codeLivre, 9); // 9 : first 4 chars and other 5 nums

    mvprintw(7, 10, "CNIE :");
    getnstr(etd.CNIE, 14); // 14 is too much but just in case
    
    if(!rendreLivre(&etd, codeLivre)){ // rendrelivre returns 0 if the operation is successful
        mvprintw(23, 4, "Retour effectue avec succes !");
    }else{
        mvprintw(23, 4, "Erreur lors du retour !");
    }
    noecho();
    curs_set(0);
    getch();
}
void afficherListeEtudiant() {
    int ch;
    chargerEtudiant("emprunts.txt", etudiants, &total_etudiants);
    
    do {
        clear();
        mvprintw(2, 35, "LISTE DES ETUDIANT (%d/%d)", start_index + 1, total_etudiants);
        
        int max_lines = LINES - 6;
        for(int i = 0; i < max_lines && (i + start_index) < total_etudiants; i++) {
            Etudiant *l = &etudiants[i + start_index];
            mvprintw(5 + i, 3, "%s - %s %s | Emprunts: %s %s %s %s %s %s %s %s %s %s",
                    l->CNIE, l->nom, l->prenom,
                    l->emprunts[0], l->emprunts[1], l->emprunts[2],
                    l->emprunts[3], l->emprunts[4], l->emprunts[5],
                    l->emprunts[6], l->emprunts[7], l->emprunts[8],
                    l->emprunts[9]
                    );
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
            case KEY_NPAGE: // Page down
                start_index += max_lines;
                if(start_index >= total_livres) start_index = total_livres - 1;
                break;
            case KEY_PPAGE: // Page up
                start_index -= max_lines;
                if(start_index < 0) start_index = 0;
                break;
        }
    } while(ch != 'R' && ch != 'r');
}