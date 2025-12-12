
typedef struct Position{
    int numeroLigne;
    int ordre;
    int numeroPhrase;
    struct Position* suivant;
}T_Position;

//A VOIR

// typedef struct Phrase{
//     int numeroPhrase;
//     char* texte;
//     struct Phrase* suivant;
// }T_Phrase;

// typedef struct ligne{
//     int numeroLigne;
//     T_Phrase* phrases[10];
//     struct ligne* suivant;
// }T_Ligne;

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

T_Position *ajouterPosition(T_Position *listeP, int ligne, int ordre, int phrase);
int ajouterOccurence(T_Index** index, char *mot, int ligne, int ordre, int phrase);
void parcourPrefixe(T_Noeud* noeud);
void parcourInfixe(T_Noeud* noeud );
int indexerFichier(T_Index **index, char *filename);