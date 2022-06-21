#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "stack.h"

/*Structure de même format qu'un automate*/
struct automat{
    unsigned char* line1;
    unsigned char* line2;
    unsigned char* line3;
    unsigned char* line4;
    unsigned char* line5;
};
typedef struct automat automat;
/*Structure d'un couple(int,char)*/
struct couple{
    int n;/*Première composante renvoyée par réduit*/
    char A;/*Deuxième composante renvoyée par réduit*/
};
typedef struct couple couple;

/*@requires f est valide
  @assigns rien
  @ensures retourne un automat de même structure et de même 
           composition que f
*/
automat data(FILE *f, int n, int size_line1){
    automat a;
    a.line1=malloc(size_line1*sizeof(unsigned char));
    if(fread(&(*a.line1), sizeof(unsigned char),size_line1,f)==-1){
        perror("Line 1 could not be read!");
        exit(127);
    }/*Récupération sans erreur de la ligne 1*/
    a.line2=malloc(n*128+1*sizeof(char));
    if(fread(&(*a.line2), sizeof(char),n*128+1,f)==-1){
        perror("Line 2 could not be read!");
        exit(127);
    }/*Récupération sans erreur de la ligne 2*/
    a.line3=malloc((n+1)*sizeof(char));
    if(fread(&(*a.line3), sizeof(char),n+1,f)==-1){
        perror("Line 3 could not be read!");
        exit(127);
    }/*Récupération sans erreur de la ligne 3*/
    a.line4=malloc((n+1)*sizeof(char));
    if(fread(&(*a.line4), sizeof(char),n+1,f)==-1){
        perror("Line 4 could not be read!");
        exit(127);
    }/*Récupération sans erreur de la ligne 4*/
    int pos=ftell(f);/*nombre de caractères lus*/
    fseek(f,0,SEEK_END);
    int size=ftell(f);/*Calcul de la taille du fichier*/
    fseek(f,pos,SEEK_SET);
    a.line5=malloc((size-pos)*sizeof(char));/*size-pos est la taille
    de la ligne 5*/
    if(fread(&(*a.line5), sizeof(char),size-pos,f)==-1){
        perror("Line 5 could not be read!");
        exit(127);
    }/*Récupération sans erreur de la ligne 5*/
    return a;
}

/*@requires donnees est un automate valide
  @assigns rien
  @ensures retourne le charactère à la position s de la ligne 2
*/
unsigned char action(automat donnees, int s, char c){
    return donnees.line2[s*128+c];
}

/*@requires donnees est un automate valide
  @assigns rien
  @ensures retourne le charactère à la position s de la ligne 3
           et le charactère à la position s de la ligne 4
*/
couple reduit(automat donnees, int s){
    couple c;
    c.n=donnees.line3[s];
    c.A=donnees.line4[s];
    return c;
}

/*@requires donnees est un automate valide
  @assigns rien
  @ensures recherche une suite de charactère "sc" sur la ligne 5 
           et renvoie le charactère suivant
*/
int decale(automat donnees, int s, char c){

    int i=0, j=i+1, k=j+1; /*Se placer respectivement sur le 1er, 2ème
    et 3ème charactère*/
    int d;/*3ème charactère*/
    while(donnees.line5[i]!=173||donnees.line5[j]!=173||donnees.line5[k]!=173){
        if(donnees.line5[i]==s && donnees.line5[j]==c)  d=donnees.line5[k];
        i+=3; j+=3; k+=3;/*Avancer de trois charactères et passer au
        prochain regroupement de trois*/
    }
    return d;
}
int branchement(automat donnees, int s, char A){
    int i=0, j=i+1, k=j+1;/*Se placer respectivement sur le 1er, 2ème
    et 3ème charactère*/
    int b;
    while(donnees.line5[i]!=173||donnees.line5[j]!=173||donnees.line5[k]!=173){ 
        i+=3; j+=3; k+=3;
    }/*Recherche du premier regroupement de trois 173*/
    i+=3; j=i+1; k=j+1;/*Se placer après le premier regroupement de
    173*/
    while(donnees.line5[i]!=173||donnees.line5[j]!=173||donnees.line5[k]!=173){
        if(donnees.line5[i]==s && donnees.line5[j]==A){
            b= donnees.line5[k];
        } 
        i+=3; j+=3; k+=3;/*Avancer de trois charactères et passer au
        prochain regroupement de trois*/
    }
    return b;
}
/*@requires donnees est un automat valide
  @assigns rien
  @ensures libère la mémoire allouée dynamiquement pour chaque ligne de donnees
*/
void free_automat(automat donnees){
    free(donnees.line1);
    free(donnees.line2);
    free(donnees.line3);
    free(donnees.line4);
    free(donnees.line5);
}

int main(int argc, char** argv){
    FILE* f;
    f=fopen(argv[1], "r");/*Ouvrir le fichier en argument en lecture*/
    if (f==NULL){
        perror("File could not be opened!");
        exit(127);
    }
    char line1[256]; char a; int n;
    if(fgets(line1,256,f)==NULL){
        perror("Line 1 could not be read!");
        exit(127);
    } 
    int size_line1=ftell(f);/*Récupération de la taille de la ligne 1*/
    sscanf(line1,"%c %i", &a, &n);/*Récupération du nombre d'états de
    l'automate n*/
    fseek(f,0,SEEK_SET);/*Retour au début du fichier*/
    automat donnees;
    donnees=data(f,n,size_line1);/*Récupération d'un automat de même
    structure et de même composition que f*/
    printf("File '%s' correctly read. Please enter your inputs\n", argv[1]);
    /*Le fichier est correctement lu si aucun fread de la fonction data ne renvoie d'erreur*/
    char buf[256];
    if(fgets(buf, 256, stdin)==NULL){
        perror("stdin could not be read!");
        exit(127);
    }/*Récupération du stdin*/
    stack state_stack;/*pile d'états*/
    init_stack(&state_stack);
    push(&state_stack,0);/*état initial=0*/
    int i, k=5;/*initialiser k>1 pour rentrer dans la boucle*/
    while(k>1){
        int s=pop(&state_stack);/*Récupération de l'état courant dans s*/
        push(&state_stack,s);
        k=action(donnees, s, buf[i]);/*Récupération de action*/
        if (k==2){/*L'action est décale*/
            push(&state_stack, decale(donnees,s,buf[i]));
            i+=1;/*Passage à la lettre suivante*/
        }
        if(k==3){/*L'action est réduit*/
            couple c=reduit(donnees,s);
            int m=c.n, j;
            char A=c.A;/*(m,A) est le couple renvoyé par reduit*/
            for(j=0; j<m+1; j+=1){/*On dépile m+1 états*/
                s=pop(&state_stack);
            }
            push(&state_stack, s);/*On empile le dernier état dépilé qui 
            devient l'état courant*/
            push(&state_stack, branchement(donnees,s,A));
        }
    }
    if (k==0){/*L'action est Rejette*/
        if (buf[i]=='\n') printf("Rejected: incorrect order of characters\n");
        else printf("Rejected: Error is here ->'%c':character number %i\n", buf[i], i+1);/*Bonus! Emplacement de l'erreur*/
        }
    if (k==1){/*L'action est Accepte*/
        printf("Accepted\n");
    }
    free(state_stack);
    free_automat(donnees);
    return 0;
}