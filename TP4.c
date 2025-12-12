#include "tp4.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

T_Position* CreerPosition(int ligne, int ordre, int phrase){
    T_Position* nouvellePosition = (T_Position*)malloc(sizeof(T_Position));
    if(nouvellePosition == NULL){
        printf("Erreur d'allocation memoire pour une nouvelle position.\n");
        exit(EXIT_FAILURE);
    }
    nouvellePosition->numeroLigne = ligne;
    nouvellePosition->ordre = ordre;
    nouvellePosition->numeroPhrase = phrase;
    nouvellePosition->suivant = NULL;
    return nouvellePosition;
}

T_Position *ajouterPosition(T_Position *listeP, int ligne, int ordre, int phrase){
    T_Position* nouvellePosition = CreerPosition(ligne,ordre,phrase);
    if(listeP == NULL){
        return nouvellePosition;
    }else{
        if (listeP->numeroLigne > ligne ||
            (listeP->numeroLigne == ligne && (listeP->ordre > ordre || (listeP->ordre == ordre && listeP->numeroPhrase >= phrase)))){
            nouvellePosition->suivant = listeP;
            return nouvellePosition;
        }

        T_Position* temp = listeP;
        while(temp->suivant != NULL &&
              (temp->suivant->numeroLigne < ligne ||
               (temp->suivant->numeroLigne == ligne && temp->suivant->ordre < ordre) ||
               (temp->suivant->numeroLigne == ligne && temp->suivant->ordre == ordre && temp->suivant->numeroPhrase < phrase))){
            temp = temp->suivant;
        }
        nouvellePosition->suivant = temp->suivant;
        temp->suivant = nouvellePosition;
        return listeP;
    }
}

void afficherPositions(T_Position* listeP){
    T_Position* temp = listeP;
    while(temp != NULL){
        printf("Ligne: %d, Ordre: %d, Phrase: %d\n", temp->numeroLigne, temp->ordre, temp->numeroPhrase);
        temp = temp->suivant;
    }
}

void afficherNoeud(T_Noeud* noeud){
    if(noeud != NULL){
        printf("|--%s\n", noeud->mot);
        T_Position* pos = noeud->listePositions;
        while(pos != NULL){
        printf("|----(l:%d ,o %d, p:%d)\n", pos->numeroLigne, pos->ordre, pos->numeroPhrase);
        pos = pos->suivant;
        }
    
    }
}

void parcourPrefixe(T_Noeud* noeud){
    if(noeud != NULL){
        afficherNoeud(noeud);
        parcourPrefixe(noeud->filsGauche);
        parcourPrefixe(noeud->filsDroit);
    }
}

int vis[28]={0};

void parcourInfixe(T_Noeud* noeud ){
    if(noeud != NULL){ 
        parcourInfixe(noeud->filsGauche);
        if(vis[noeud->mot[0] -'a']==0){
            vis[noeud->mot[0] -'a']=1;
            printf("Lettre %c:\n", noeud->mot[0]-(32));
        }
        afficherNoeud(noeud);
        parcourInfixe(noeud->filsDroit);
    }
}



static T_Noeud* rechercherMot(T_Noeud* racine, char *mot){
    T_Noeud* courant = racine;
    while(courant != NULL){
        int cmp = strcmp(courant->mot, mot);
        if(cmp == 0){
            return courant;
        }else if(cmp > 0){
            courant = courant->filsGauche;
        }else{
            courant = courant->filsDroit;
        }
    }
    return NULL;
}

int ajouterOccurence(T_Index** index, char *mot, int ligne, int ordre, int phrase){
    if(*index ==NULL){
            *index = (T_Index*)malloc(sizeof(T_Index));
            T_Noeud* newNode = (T_Noeud*)malloc(sizeof(T_Noeud));
            if(newNode == NULL){
                printf("Erreur d'allocation memoire pour un nouveau noeud.\n");
                return 0;
            }
            newNode->mot = mot;
            newNode->nbOccurrences = 1;
            newNode->listePositions = CreerPosition(ligne,ordre,phrase);
            newNode->filsGauche = NULL;
            newNode->filsDroit = NULL;
            (*index)->racine = newNode;
            (*index)->nbMotsDistincts = 1;
            (*index)->nbMotsTotal = 1;
            return 1;
    }
    else {
        T_Index* tmp = *index;
        T_Noeud* current = tmp->racine;
        T_Noeud* parent = NULL;

        while (current!=NULL)
        {
           parent = current;
           int cmp = strcmp(current->mot, mot);
           if(cmp == 0){
               current->nbOccurrences +=1;
               current->listePositions = ajouterPosition(current->listePositions,ligne,ordre,phrase);
               tmp->nbMotsTotal += 1;
               return 1;
           }
           else if(cmp > 0){
               current = current->filsGauche;
           }
           else{
               current = current->filsDroit;
           }
        }

        if(current == NULL){
            T_Noeud* newNode = (T_Noeud*)malloc(sizeof(T_Noeud));
            if(newNode == NULL){
                printf("Erreur d'allocation memoire pour un nouveau noeud.\n");
                return 0;
            }
            newNode->mot = mot;
            newNode->nbOccurrences = 1;
            newNode->listePositions = CreerPosition(ligne,ordre,phrase);
            newNode->filsGauche = NULL;
            newNode->filsDroit = NULL;

            if(strcmp(parent->mot, mot) > 0){
                parent->filsGauche = newNode;
            }
            else{
                parent->filsDroit = newNode;
            }
            tmp->nbMotsDistincts += 1;
            tmp->nbMotsTotal += 1;
            return 1;
        }
        
    }
}

int indexerFichier(T_Index **index, char *filename){
    FILE *fichier = fopen(filename, "r");
    if(fichier == NULL){
        printf("Erreur: impossible d'ouvrir le fichier %s\n", filename);
        return 0;
    }
    
    int nbMotsLus = 0;
    int numeroLigne = 1;
    int ordre = 1;
    int numeroPhrase = 1;
    char mot[100];
    int c;
    int i = 0;
    
    while((c = fgetc(fichier)) != EOF){
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')){
            // Caractère alphanumérique, on l'ajoute au mot
            if(i < 99){
                mot[i++] = c;
            }
        }
        else{
            // Fin d'un mot
            if(i > 0){
                mot[i] = '\0';
                
                // Convertir le mot en minuscules
                for(int j = 0; j < i; j++){
                    if(mot[j] >= 'A' && mot[j] <= 'Z'){
                        mot[j] = mot[j] + 32;
                    }
                }
                
                // Allouer de la mémoire pour le mot et le copier
                char *motCopie = (char*)malloc((strlen(mot) + 1) * sizeof(char));
                if(motCopie == NULL){
                    printf("Erreur d'allocation memoire pour le mot.\n");
                    fclose(fichier);
                    return nbMotsLus;
                }
                strcpy(motCopie, mot);
                
                // Ajouter le mot à l'index
                ajouterOccurence(index, motCopie, numeroLigne, ordre, numeroPhrase);
                nbMotsLus++;
                ordre++;
                i = 0;
            }
            
            // Gérer les sauts de ligne et les phrases
            if(c == '\n'){
                numeroLigne++;
                ordre = 1;
            }
            else if(c == '.' || c == '!' || c == '?'){
                numeroPhrase++;
            }
        }
    }
    
    // Traiter le dernier mot si le fichier ne se termine pas par un séparateur
    if(i > 0){
        mot[i] = '\0';
        
        // Convertir le mot en minuscules
        for(int j = 0; j < i; j++){
            if(mot[j] >= 'A' && mot[j] <= 'Z'){
                mot[j] = mot[j] + 32;
            }
        }
        
        char *motCopie = (char*)malloc((strlen(mot) + 1) * sizeof(char));
        if(motCopie != NULL){
            strcpy(motCopie, mot);
            ajouterOccurence(index, motCopie, numeroLigne, ordre, numeroPhrase);
            nbMotsLus++;
        }
    }
    
    fclose(fichier);
    return nbMotsLus;
}

int main(){

    T_Index* index = NULL;
    int nbMotsLus = indexerFichier(&index, "texte.txt");
    printf("Nombre total de mots indexes: %d\n", nbMotsLus);
    parcourInfixe(index->racine);
    return 0;
}