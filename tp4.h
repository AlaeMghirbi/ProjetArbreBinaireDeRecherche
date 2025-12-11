
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