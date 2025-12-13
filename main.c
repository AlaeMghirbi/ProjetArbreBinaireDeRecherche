#include "tp4.h"

void afficherMenu() {
    printf("\n");
    printf("========================================\n");
    printf("       MENU - INDEXATION DE TEXTE      \n");
    printf("========================================\n");
    printf("1. Charger un fichier\n");
    printf("2. Afficher l'index\n");
    printf("3. Afficher les occurrences d'un mot\n");
    printf("4. Construire le texte a partir de l'index\n");
    printf("5. Quitter\n");
    printf("========================================\n");
    printf("Votre choix : ");
}


int main() {
    T_Index* index = NULL;
    int choix;
    int indexCharge = 0;

    do {
        afficherMenu();

        if(scanf("%d", &choix) != 1) {
            // Vider le buffer en cas d'erreur de saisie
            while(getchar() != '\n');
            printf("\nEntree invalide. Veuillez entrer un nombre.\n");
            continue;
        }

        switch(choix) {
            case 1: {
                // Charger un fichier
                char nomFichier[256];
                printf("\nEntrez le nom du fichier a indexer : ");
                scanf("%s", nomFichier);

                // Si un index existe deja, le liberer
                if(indexCharge) {
                    libererIndex(index);
                    index = NULL;
                }

                printf("\nChargement du fichier '%s'...\n", nomFichier);
                int nbMots = indexerFichier(&index, nomFichier);

                if(nbMots > 0) {
                    printf(" %d mots ont ete lus et indexes.\n", nbMots);
                    indexCharge = 1;
                } else {
                    printf("Erreur lors du chargement du fichier.\n");
                    indexCharge = 0;
                }
                break;
            }

            case 2: {
                // Afficher l'index
                if(!indexCharge) {
                    printf("\nVeuillez d'abord charger un fichier (option 1).\n");
                } else {
                    printf("\n=== Affichage de l'index ===\n");
                    afficherIndex(*index);
                    printf("=================================\n");
                }
                break;
            }

            case 3: {
                // Afficher les occurrences d'un mot
                if(!indexCharge) {
                    printf("\nVeuillez d'abord charger un fichier (option 1).\n");
                } else {
                    char mot[100];
                    printf("\nEntrez le mot dont vous voulez afficher les occurrences : ");
                    scanf("%s", mot);

                    printf("\n=== Recherche des occurrences ===\n");
                    afficherOccurencesMot(*index, mot);
                    printf("=================================\n");
                }
                break;
            }

            case 4: {
                // Construire le texte
                if(!indexCharge) {
                    printf("\nVeuillez d'abord charger un fichier (option 1).\n");
                } else {
                    char nomFichierSortie[256];
                    printf("\nEntrez le nom du fichier de sortie : ");
                    scanf("%s", nomFichierSortie);

                    printf("\nConstruction du texte...\n");
                    construireTexte(*index, nomFichierSortie);
                }
                break;
            }

            case 5: {
                // Quitter
                printf("\nLiberation de la memoire...\n");
                if(indexCharge) {
                    libererIndex(index);
                }
                printf("Au revoir !\n\n");
                break;
            }

            default:
                printf("\nChoix invalide. Veuillez choisir une option entre 1 et 7.\n");
                break;
        }

    } while(choix != 5);

    return 0;
}
