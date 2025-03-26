#include <stdio.h>
#include "structs.h"

int livre_read(char *name) {
    FILE *pf=fopen(name,"r");
    if (pf==NULL) {
        printf("Erreur d'ouverture du fichier %s\n",name);
        return 0;
    }
    while(!feof(pf)) {
        livre l;
        fscanf(pf,"%s %s %s %d %d %d\n",l.code,l.titre,l.auteur,&l.annee,&l.nbExemplaires,&l.nbExemplairesDisponibles);
        printf("%s %s %s %d %d %d\n",l.code,l.titre,l.auteur,l.annee,l.nbExemplaires,l.nbExemplairesDisponibles);
    }
    return 1;
}

int etudiant_read(char *name) {
    FILE *pf=fopen(name,"r");
    if (pf==NULL) {
        printf("Erreur d'ouverture du fichier %s\n",name);
        return 0;
    }
    while(!feof(pf)) {
        etudiant e;
        fscanf(pf,"%s %s %s\n",e.prenom,e.nom,e.CNIE);
        printf("%s %s %s\n",e.prenom,e.nom,e.CNIE);
    }
    return 1;
}

int livre_write(char *name) {
    FILE *pf=fopen(name,"w");
    if (pf==NULL) {
        printf("Erreur d'ouverture du fichier %s\n",name);
        return 0;
    }
    livre l;
    printf("Donner le code du livre: ");
    scanf("%s",l.code);
    printf("Donner le titre du livre: ");
    scanf("%s",l.titre);
    printf("Donner l'auteur du livre: ");
    scanf("%s",l.auteur);
    printf("Donner l'annee du livre: ");
    scanf("%d",&l.annee);
    printf("Donner le nombre d'exemplaires du livre: ");
    scanf("%d",&l.nbExemplaires);
    printf("Donner le nombre d'exemplaires disponibles du livre: ");
    scanf("%d",&l.nbExemplairesDisponibles);
    fprintf(pf,"%s %s %s %d %d %d\n",l.code,l.titre,l.auteur,l.annee,l.nbExemplaires,l.nbExemplairesDisponibles);
    fclose(pf);
    return 1;
}

int etudiant_write(char *name) {
    FILE *pf=fopen(name,"w");
    if (pf==NULL) {
        printf("Erreur d'ouverture du fichier %s\n",name);
        return 0;
    }
    etudiant e;
    printf("Donner le prenom de l'etudiant: ");
    scanf("%s",e.prenom);
    printf("Donner le nom de l'etudiant: ");
    scanf("%s",e.nom);
    printf("Donner le CNIE de l'etudiant: ");
    scanf("%s",e.CNIE);
    fprintf(pf,"%s %s %s\n",e.prenom,e.nom,e.CNIE);
    fclose(pf);
    return 1;
}