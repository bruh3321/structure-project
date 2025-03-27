#include <stdio.h>
#include "customlibs/storagemanagement.h"
#include "customlibs/structs.h"
#define TRUE 1
#define FALSE 0
#define MAX 80


void help(){
    printf("---------------------------------------------\n");
    printf("1.Ajouter un livre\n");
    printf("2.recherch livres\n");
    printf("3.Emprunter un livre\n");
    printf("4.Rendre un livre\n");
    printf("5.Quitter\n");
    printf("---------------------------------------------\n");
}


int main(){
    printf("---------------------------------------------\n");
    printf("Bienvenue dans le programme de gestion de la bibliotheque\n");
    printf("---------------------------------------------\n");
    int choix;
    int state = TRUE;
    help();
    while (state){
        printf("Donner votre choix: ");
        scanf("%d",&choix);
        switch (choix){
        case 1: // Ajouter un livre
            livre_write("./data/livres.txt");
            break;
        case 2: // Rechercher un livre
        
        default:
            break;
        }
    }
    
}