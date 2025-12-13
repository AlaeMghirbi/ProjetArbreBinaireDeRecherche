#ifndef TP4_H
#define TP4_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// STRUCTURES

typedef struct Position{
    int numeroLigne;
    int ordre;
    int numeroPhrase;
    struct Position* suivant;
}T_Position;

typedef struct Noeud{
    char* mot;
    int nbOccurrences;
    T_Position* listePositions;
    struct Noeud* filsGauche;
    struct Noeud* filsDroit;
}T_Noeud;

typedef struct Index{
    T_Noeud* racine;
    int nbMotsDistincts;
    int nbMotsTotal;
}T_Index;

// STRUCTURES ADDITIONNELLE POUR LISTE DE PHRASES

// Structure pour un mot dans une phrase
typedef struct MotPhrase {
    char* mot;
    int numeroLigne;
    int ordre;
    struct MotPhrase* suivant;
} T_MotPhrase;

// Structure pour une phrase
typedef struct Phrase {
    T_MotPhrase* listeMots;
    int numeroPhrase;
    struct Phrase* suivant;
} T_Phrase;




// FONCTIONS

// Gestion des positions
T_Position* CreerPosition(int ligne, int ordre, int phrase);
T_Position* ajouterPosition(T_Position *listeP, int ligne, int ordre, int phrase);

// Gestion de l'index (ABR)
int ajouterOccurence(T_Index** index, char *mot, int ligne, int ordre, int phrase);
int indexerFichier(T_Index **index, char *filename);

// Affichage
void afficherPositions(T_Position* listeP);
void afficherNoeud(T_Noeud* noeud);
void parcourInfixe(T_Noeud* noeud);
void afficherIndex(T_Index index);

// Recherche mot
T_Noeud* rechercherMot(T_Index index, char *mot);

// FONCTIONS POUR LA LISTE DE PHRASES
T_MotPhrase* creerMotPhrase(char* mot, int ligne, int ordre);
T_Phrase* creerPhrase(int numeroPhrase);
void ajouterMotDansPhrase(T_Phrase* phrase, char* mot, int ligne, int ordre);
T_Phrase* ajouterPhrase(T_Phrase* listePhrases, T_Phrase* nouvellePhrase);
T_Phrase* trouverOuCreerPhrase(T_Phrase** listePhrases, int numeroPhrase);
void parcourirArbrePourPhrases(T_Noeud* noeud, T_Phrase** listePhrases);
T_Phrase* construireListePhrases(T_Index index);
void afficherPhrase(T_MotPhrase* listeMots);


// Afficher toutes les phrases contenant un mot
void afficherOccurencesMot(T_Index index, char *mot);

// Reconstruire le texte à partir de l'index
void construireTexte(T_Index index, char *filename);

// Libération de mémoire
void libererPositions(T_Position* position);
void libererArbre(T_Noeud* noeud);
void libererIndex(T_Index* index);
void libererListePhrases(T_Phrase* listePhrases);



// VARIABLE GLOBALE

// Tableau pour l'affichage par lettre (parcours infixe)
extern int visited[27];

#endif