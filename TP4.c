#include "tp4.h"

// ==================== GESTION DES POSITIONS ====================

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
        // Insertion en tête si nécessaire
        if (listeP->numeroLigne > ligne ||
            (listeP->numeroLigne == ligne && (listeP->ordre > ordre || (listeP->ordre == ordre && listeP->numeroPhrase >= phrase)))){
            nouvellePosition->suivant = listeP;
            return nouvellePosition;
        }

        // Recherche de la position d'insertion
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

// ==================== GESTION DE L'INDEX (ABR) ====================

int ajouterOccurence(T_Index** index, char *mot, int ligne, int ordre, int phrase){
    // Création de l'index si vide
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

        // Recherche du mot dans l'ABR
        while (current!=NULL)
        {
           parent = current;
           int cmp = strcmp(current->mot, mot);
           if(cmp == 0){
               // Mot trouvé : ajout d'une nouvelle occurrence
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

        // Mot non trouvé : création d'un nouveau noeud
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
    return 0;
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
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
            if(i < 99){
                mot[i++] = c;
            }
        }
        else{
            // Fin d'un mot
            if(i > 0){
                mot[i] = '\0';

                // Conversion en minuscules
                for(int j = 0; j < i; j++){
                    if(mot[j] >= 'A' && mot[j] <= 'Z'){
                        mot[j] = mot[j] + 32;
                    }
                }

                char *motCopie = (char*)malloc((strlen(mot) + 1) * sizeof(char));
                if(motCopie == NULL){
                    printf("Erreur d'allocation memoire pour le mot.\n");
                    fclose(fichier);
                    return nbMotsLus;
                }
                strcpy(motCopie, mot);

                ajouterOccurence(index, motCopie, numeroLigne, ordre, numeroPhrase);
                nbMotsLus++;
                ordre++;
                i = 0;
            }

            // Gestion sauts de ligne et séparateurs de phrases
            if(c == '\n'){
                numeroLigne++;
                ordre = 1;
            }
            else if(c == '.' || c == '!' || c == '?'){
                numeroPhrase++;
            }
        }
    }

    // Traitement du dernier mot
    if(i > 0){
        mot[i] = '\0';
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

// ==================== AFFICHAGE DE L'INDEX ====================

void afficherPositions(T_Position* listeP){
    T_Position* temp = listeP;
    while(temp != NULL){
        printf("|----(l:%d ,o %d, p:%d)\n", temp->numeroLigne, temp->ordre, temp->numeroPhrase);
        temp = temp->suivant;
    }
}

void afficherNoeud(T_Noeud* noeud){
    if(noeud != NULL){
        printf("|--%s\n", noeud->mot);
        afficherPositions(noeud->listePositions);
    }
}

// Tableau pour afficher les lettres une seule fois
int visited[27]={0};

// Parcours infixe avec affichage par ordre alphabétique
void parcourInfixe(T_Noeud* noeud ){
    if(noeud != NULL){
        parcourInfixe(noeud->filsGauche);
        if(visited[noeud->mot[0] -'a']==0){
            visited[noeud->mot[0] -'a']=1;
            printf("%c\n", noeud->mot[0]-(32));
        }
        afficherNoeud(noeud);
        parcourInfixe(noeud->filsDroit);
    }
}

void afficherIndex(T_Index index) {
    if(index.racine == NULL) {
        printf("\nL'index est vide.\n");
        return;
    }

    printf("Nombre de mots distincts : %d\n", index.nbMotsDistincts);
    printf("Nombre total de mots : %d\n", index.nbMotsTotal);

    // Réinitialisation du tableau visited
    for (int i=0; i < 27; i++) {
        visited[i] = 0;
    }
    parcourInfixe(index.racine);
}

// ==================== RECHERCHE MOT ====================

T_Noeud* rechercherMot(T_Index index, char *mot){
    T_Noeud* courant = index.racine;
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

// ==================== CONSTRUCTION LISTE DE PHRASES ====================

T_MotPhrase* creerMotPhrase(char* mot, int ligne, int ordre) {
    T_MotPhrase* nouveauMot = (T_MotPhrase*)malloc(sizeof(T_MotPhrase));
    if(nouveauMot == NULL) {
        printf("Erreur d'allocation memoire pour un mot de phrase.\n");
        return NULL;
    }
    nouveauMot->mot = mot;
    nouveauMot->numeroLigne = ligne;
    nouveauMot->ordre = ordre;
    nouveauMot->suivant = NULL;
    return nouveauMot;
}

T_Phrase* creerPhrase(int numeroPhrase) {
    T_Phrase* nouvellePhrase = (T_Phrase*)malloc(sizeof(T_Phrase));
    if(nouvellePhrase == NULL) {
        printf("Erreur d'allocation memoire pour une phrase.\n");
        return NULL;
    }
    nouvellePhrase->listeMots = NULL;
    nouvellePhrase->numeroPhrase = numeroPhrase;
    nouvellePhrase->suivant = NULL;
    return nouvellePhrase;
}

// Ajoute un mot dans une phrase en maintenant l'ordre (ligne, ordre)
void ajouterMotDansPhrase(T_Phrase* phrase, char* mot, int ligne, int ordre) {
    T_MotPhrase* prec = NULL;
    T_MotPhrase* next = phrase->listeMots;

    // Recherche de la position d'insertion
    while (next != NULL && next->numeroLigne < ligne) {
        prec = next;
        next = next->suivant;
    }
    while (next != NULL && next->numeroLigne == ligne && next->ordre < ordre) {
        prec = next;
        next = next->suivant;
    }

    T_MotPhrase* nouveauMot = creerMotPhrase(mot, ligne, ordre);
    if(nouveauMot == NULL) {
        return;
    }

    // Insertion
    nouveauMot->suivant = next;
    if (prec == NULL) {
        phrase->listeMots = nouveauMot;
    } else {
        prec->suivant = nouveauMot;
    }
}

// Ajoute une phrase dans la liste triée par numéro de phrase
T_Phrase* ajouterPhrase(T_Phrase* listePhrases, T_Phrase* nouvellePhrase) {
    T_Phrase* prec = NULL;
    T_Phrase* next = listePhrases;
    while (next != NULL && next->numeroPhrase < nouvellePhrase->numeroPhrase) {
        prec = next;
        next = next->suivant;
    }
    nouvellePhrase->suivant = next;
    if (prec == NULL) {
        listePhrases = nouvellePhrase;
    } else {
        prec->suivant = nouvellePhrase;
    }
    return listePhrases;
}

// Trouve une phrase ou la crée si elle n'existe pas
T_Phrase* trouverOuCreerPhrase(T_Phrase** listePhrases, int numeroPhrase) {
    T_Phrase* courant = *listePhrases;

    while(courant != NULL) {
        if(courant->numeroPhrase == numeroPhrase) {
            return courant;
        }
        courant = courant->suivant;
    }

    T_Phrase* nouvellePhrase = creerPhrase(numeroPhrase);
    if(nouvellePhrase != NULL) {
        *listePhrases = ajouterPhrase(*listePhrases, nouvellePhrase);
    }
    return nouvellePhrase;
}

// Parcours récursif de l'ABR pour construire la liste de phrases
void parcourirArbrePourPhrases(T_Noeud* noeud, T_Phrase** listePhrases) {
    if(noeud == NULL) {
        return;
    }

    parcourirArbrePourPhrases(noeud->filsGauche, listePhrases);

    // Pour chaque position du mot, l'ajouter à sa phrase
    T_Position* pos = noeud->listePositions;
    while(pos != NULL) {
        T_Phrase* phrase = trouverOuCreerPhrase(listePhrases, pos->numeroPhrase);
        if(phrase != NULL) {
            ajouterMotDansPhrase(phrase, noeud->mot, pos->numeroLigne, pos->ordre);
        }
        pos = pos->suivant;
    }

    parcourirArbrePourPhrases(noeud->filsDroit, listePhrases);
}

// Construit la liste complète de toutes les phrases du texte
T_Phrase* construireListePhrases(T_Index index) {
    if(index.racine == NULL) {
        return NULL;
    }

    T_Phrase* listePhrases = NULL;
    parcourirArbrePourPhrases(index.racine, &listePhrases);

    return listePhrases;
}

void afficherPhrase(T_MotPhrase* listeMots) {
    T_MotPhrase* motActuel = listeMots;
    while(motActuel != NULL) {
        printf("%s", motActuel->mot);
        if(motActuel->suivant != NULL) {
            printf(" ");
        }
        motActuel = motActuel->suivant;
    }
    printf(".\n");
}

// ==================== AFFICHER OCCURRENCES ====================

void afficherOccurencesMot(T_Index index, char *mot) {
    if(index.racine == NULL) {
        printf("L'index est vide.\n");
        return;
    }

    // Conversion en minuscules
    char motMinuscule[100];
    int i = 0;
    while(mot[i] != '\0' && i < 99) {
        if(mot[i] >= 'A' && mot[i] <= 'Z') {
            motMinuscule[i] = mot[i] + 32;
        } else {
            motMinuscule[i] = mot[i];
        }
        i++;
    }
    motMinuscule[i] = '\0';

    T_Noeud* noeud = rechercherMot(index, motMinuscule);
    if(noeud == NULL) {
        printf("Le mot \"%s\" n'a pas été trouvé dans l'index.\n", mot);
        return;
    }

    printf("Mot = \"%s\"\n", noeud->mot);
    printf("Occurences = %d\n", noeud->nbOccurrences);

    // Construction de la liste de phrases (une seule fois)
    T_Phrase* listePhrases = construireListePhrases(index);

    // Affichage de chaque phrase contenant le mot
    T_Position* pos = noeud->listePositions;
    while(pos != NULL) {
        int numeroPhrase = pos->numeroPhrase;
        int ligneMotRecherche = pos->numeroLigne;
        int ordreMotRecherche = pos->ordre;

        // Recherche de la phrase correspondante
        T_Phrase* phraseActuelle = listePhrases;
        while(phraseActuelle != NULL && phraseActuelle->numeroPhrase != numeroPhrase) {
            phraseActuelle = phraseActuelle->suivant;
        }

        if(phraseActuelle != NULL) {
            printf("| Ligne %d, mot %d : ", ligneMotRecherche, ordreMotRecherche);
            afficherPhrase(phraseActuelle->listeMots);
        }

        pos = pos->suivant;
    }

    libererListePhrases(listePhrases);
}

// ====================  CONSTRUIRE TEXTE ====================

void construireTexte(T_Index index, char *filename) {
    if(index.racine == NULL) {
        printf("L'index est vide. Impossible de construire le texte.\n");
        return;
    }

    FILE* fichier = fopen(filename, "w");
    if(fichier == NULL) {
        printf("Erreur: impossible de créer le fichier %s\n", filename);
        return;
    }

    T_Phrase* listePhrases = construireListePhrases(index);
    if(listePhrases == NULL) {
        printf("Erreur lors de la construction de la liste de phrases.\n");
        fclose(fichier);
        return;
    }

    T_Phrase* phraseActuelle = listePhrases;
    int ligneActuelle = 1;

    while(phraseActuelle != NULL) {
        T_MotPhrase* motActuel = phraseActuelle->listeMots;
        int premierMotPhrase = 1;

        while(motActuel != NULL) {
            // Gestion des sauts de ligne
            while(ligneActuelle < motActuel->numeroLigne) {
                fprintf(fichier, "\n");
                ligneActuelle++;
            }

            // Écriture du mot (majuscule si début de phrase)
            if(premierMotPhrase) {
                char premiereMajuscule = motActuel->mot[0];
                if(premiereMajuscule >= 'a' && premiereMajuscule <= 'z') {
                    premiereMajuscule -= 32;
                }
                fprintf(fichier, "%c%s", premiereMajuscule, motActuel->mot + 1);
                premierMotPhrase = 0;
            } else {
                fprintf(fichier, "%s", motActuel->mot);
            }

            if(motActuel->suivant != NULL) {
                fprintf(fichier, " ");
            }

            motActuel = motActuel->suivant;
        }

        // Point de fin de phrase
        fprintf(fichier, ".");

        // Espace après le point si la phrase suivante est sur la même ligne
        if(phraseActuelle->suivant != NULL) {
            T_MotPhrase* premierMotPhraseSuivante = phraseActuelle->suivant->listeMots;
            if(premierMotPhraseSuivante != NULL &&
               premierMotPhraseSuivante->numeroLigne == ligneActuelle) {
                fprintf(fichier, " ");
            }
        }

        phraseActuelle = phraseActuelle->suivant;
    }

    fprintf(fichier, "\n");
    libererListePhrases(listePhrases);
    fclose(fichier);

    printf("Le texte a ete construit et enregistre dans le fichier '%s'.\n", filename);
}

// ==================== LIBÉRATION MÉMOIRE ====================

void libererPositions(T_Position* position) {
    if (position == NULL) {
        return;
    }
    libererPositions(position->suivant);
    free(position);
}

void libererArbre(T_Noeud* noeud) {
    if(noeud == NULL) {
        return;
    }
    libererArbre(noeud->filsGauche);
    libererArbre(noeud->filsDroit);
    libererPositions(noeud->listePositions);
    free(noeud->mot);
    free(noeud);
}

void libererIndex(T_Index* index) {
    if(index == NULL) {
        return;
    }
    libererArbre(index->racine);
    free(index);
}

void libererListeMot(T_MotPhrase* listeMots) {
    if (listeMots == NULL)
        return;
    libererListeMot(listeMots->suivant);
    free(listeMots);
}

void libererListePhrases(T_Phrase* listePhrases) {
    if (listePhrases == NULL)
        return;
    libererListePhrases(listePhrases->suivant);
    libererListeMot(listePhrases->listeMots);
    free(listePhrases);
}
