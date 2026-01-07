# Rapport Technique : Système d'Indexation de Texte par Arbre Binaire de Recherche

**Projet :** Indexation et Reconstruction de Texte  

**Réalisateur :** Rania Laffet, Mghirbi Alae 

**Date :** Décembre 2025  

---

## 1. Introduction:
### 1.1 Contexte du Projet
Ce projet implémente un système d'indexation de fichiers texte utilisant une structure d'**Arbre Binaire de Recherche (ABR)** pour stocker et récupérer les mots d'un document. Le système permet :

- **Indexation** : Lecture d'un fichier texte et extraction des mots avec leurs positions
- **Recherche** : Localisation rapide des mots dans le document
- **Affichage** : Présentation alphabétique des mots avec leurs occurrences
- **Reconstruction** : Regeneration du texte original à partir de l'index

### 1.2 Justification des Structures de Données supplémentaires


Au départ, le système stockait uniquement les positions isolées (ligne, ordre, numéro phrase) dans les nœuds de l'ABR. Cependant, cette approche présentait un problème majeur de compemxité tres elevé pour la construction de phrase: 

**Problème Initial :**
- Les positions individuelles ne permettaient pas  récupérer facilement "toutes les mots d'une phrase donnée"

**Solution : Structures T_Phrase et T_MotPhrase**

Ces structures créent le même index des mots organisée par phrase :

```
ABR (par mot)                 Liste de Phrases
├─ "bonjour"                  ├─ Phrase 1
│  └─ Positions              │  ├─ "bonjour"
├─ "monde"                   │  ├─ "le"
│  └─ Positions              │  └─ "monde"
└─ "le"                       └─ Phrase 2
   └─ Positions                 ├─ "le"
                                └─ "monde"
```

## 2. Architecture et Structure de Données

### 2.1 Structures Principales

#### T_Position
```c
typedef struct Position {
    int numeroLigne;      // Ligne du document (1-indexed)
    int ordre;            // Position du mot dans la ligne
    int numeroPhrase;     // Identifiant de la phrase (incrémenté à chaque . ! ?)
    struct Position* suivant;
} T_Position;
```
**Rôle :** Stocke la localisation complète d'une occurrence d'un mot dans le document.

#### T_Noeud (Arbre Binaire de Recherche)
```c
typedef struct Noeud {
    char* mot;                    
    int nbOccurrences;            
    T_Position* listePositions;   
    struct Noeud *filsGauche;    
    struct Noeud *filsDroit;    
    } T_Noeud;
```

#### T_Index
```c
typedef struct Index {
    T_Noeud* racine;          
    int nbMotsDistincts;      
    int nbMotsTotal;          
} T_Index;
```
**Rôle :** Conteneur principal pour l'index complet.

#### T_MotPhrase : (Nouvelle Structure)
```c
typedef struct MotPhrase {
    char* mot;                
    int numeroLigne;          
    int ordre;                
    struct MotPhrase* suivant; 
} T_MotPhrase;
```
**Rôle :** Représente un mot au sein d'une phrase spécifique, **en ordre séquentiel**.

#### T_Phrase : (Nouvelle Structure)
```c
typedef struct Phrase {
    T_MotPhrase* listeMots;   // Premiers mots de la phrase (chaîne ordonnée)
    int numeroPhrase;         
    struct Phrase* suivant;   
} T_Phrase;
```
**Rôle :** Regroupe tous les mots d'une phrase dans l'ordre d'apparition.

---

## 3. Analyse Complexité des Fonctions

### 3.1 Gestion des Positions

#### `CreerPosition(int ligne, int ordre, int phrase)`
- **Complexité :** **O(1)**
- **Description :** Alloue et initialise une nouvelle structure Position.

#### `ajouterPosition(T_Position *listeP, int ligne, int ordre, int phrase)`
- **Complexité :** **O(n)** où n = nombre de positions du mot
- **Justification :** Parcours de la liste chaînée jusqu'à trouver le point d'insertion correct.
- **Description :** Insère une nouvelle position dans la liste triée (par ligne, puis ordre, puis phrase).


---

### 3.2 Gestion de l'Index (ABR)

#### `ajouterOccurence(T_Index** index, char *mot, int ligne, int ordre, int phrase)`
- **Complexité  :** **O(log n)** moyen, **O(n)** pire cas (ABR dégénéré)
- **Description :** Insère un mot dans l'ABR ou incrémente son compteur d'occurrences.
- **Justification :** 
  - Recherche dichotomique dans l'ABR jusqu'à la feuille : O(n)
  - Appel à `ajouterPosition()` : O(positions du mot)
  - Total : O( n + positions) or n>=positions ( en générale)

#### `indexerFichier(T_Index **index, char *filename, T_Phrase** listePhrases)`
- **Complexité Temporelle :** **O(m × (n + p))**
  - m = nombre de caractères du fichier
  - n = nombre de mots distincts
  - Reconstruit la liste de phrases : O(n × p) où p = nombre de positions max d'un mot dans l'index
- **Justification :** 
  - Boucle char par char : O(m)
  - Chaque mot : appel `ajouterOccurence()` → O(n)
  - À la fin : appel `construireListePhrases()` → voir section 3.4
- **Description :** Lecture complète du fichier, extraction des mots, construction de l'index et des phrases.


---

### 3.3 Affichage et Recherche

#### `parcourInfixe(T_Noeud* noeud)`
- **Complexité Temporelle :** **O(n)** où n = nombre de nœuds
- **Description :** Parcours infixe (gauche → nœud → droit) pour affichage alphabétique.
- **Justification :** Visite chaque nœud exactement une fois.

#### `afficherIndex(T_Index index)`
- **Complexité Temporelle :** **O(n)** où n = mots distincts
- **Description :** Affiche l'index complet avec statistiques et tri alphabétique par lettre.

#### `rechercherMot(T_Index index, char *mot)`
- **Complexité Temporelle :** **O(log n)** moyen, **O(n)** pire cas
- **Justification :** Exploitation de la propriété ABR (comparaison strcmp et navigation gauche/droit).
- **Description :** Recherche dichotomique d'un mot dans l'ABR.


#### `afficherOccurencesMot(T_Index index, T_Phrase* listePhrases, char *mot)`
- **Complexité Temporelle :** **O( n + p + f)** 
  - n = recherche du mot
  - p = nombre de positions du mot
  - f = nombre de phrases pour accès
- **Description :** Trouve un mot et affiche toutes ses occurrences avec les phrases complètes.

---

### 3.4 Gestion des Phrases (Structures Nouvelles)

#### `creerMotPhrase(char* mot, int ligne, int ordre)`
- **Complexité Temporelle :** **O(1)**
- **Description :** Crée une entrée mot pour une phrase.

#### `creerPhrase(int numeroPhrase)`
- **Complexité Temporelle :** **O(1)**
- **Complexité Spatiale :** **O(1)**
- **Description :** Initialise une nouvelle structure Phrase.

#### `ajouterMotDansPhrase(T_Phrase* phrase, char* mot, int ligne, int ordre)`
- **Complexité Temporelle :** **O(m)** où m = mots déjà dans la phrase
- **Justification :** Parcours de la liste chaînée jusqu'au point d'insertion.
- **Description :** Insère un mot dans la liste ordonnée des mots d'une phrase.


#### `trouverOuCreerPhrase(T_Phrase** listePhrases, int numeroPhrase)`
- **Complexité Temporelle :** **O(p)** où p = nombre de phrases
- **Justification :** Parcours de la liste de phrases jusqu'au bon numéro.
- **Description :** Localise une phrase ou en crée une nouvelle si nécessaire.


#### `construireListePhrases(T_Noeud* noeud, T_Phrase* listePhrases)` 
- **Complexité Temporelle :** **O(n × m)** où n = mots, m = max(occurrences mot dans l'index)
- **Complexité Spatiale :** **O(p × w)** où p = phrases, w = mots/phrase
- **Description :** Parcours récursif inorder de l'ABR pour distribuer chaque mot dans sa(ses) phrase(s).
- **Justification :** 
  - Visite chaque nœud : O(n)
  - Pour chaque nœud, traite ses positions : O(m)
  - Crée/trouveLe des phrases et ajoute les mots : O(m)
  - Opération **duale** : construit la vue "phrases" de l'index

---

### 3.5 Reconstruction et Libération

#### `construireTexte(T_Index index, char *filename, T_Phrase* listePhrases)`
- **Complexité Temporelle :** **O(w)** où w = nombre total de mots
- **Description :** Reconstruit le texte original à partir de la liste de phrases.
- **Justification :** Parcours séquentiel de toutes les phrases et leurs mots, écriture fichier O(1) par mot.

#### `libererPositions(T_Position* position)`
- **Complexité Temporelle :** **O(n)** où n = positions
- **Description :** Libère récursivement toute une chaîne de positions.

#### `libererArbre(T_Noeud* noeud)`
- **Complexité Temporelle :** **O(n)** où n = nœuds de l'ABR
- **Description :** Parcours postfixe pour libérer tous les nœuds et leurs positions.

#### `libererListePhrases(T_Phrase* listePhrases)`
- **Complexité Temporelle :** **O(p × m)** où p = phrases, m = mots/phrase
- **Description :** Libère récursivement toutes les phrases et leurs listes de mots.

---

## 4. Conclusion et Bilan

### 4.1 Synthèse de la Complexité

| Opération | Complexité Temps 
|-----------|------------------
| Insertion mot | O(log n) moyen et O(n) pire des cas 
| Indexation fichier | O(m × (n+p))
| Recherche mot | O(log n) moyen O(n) pire des cas | 
| Affichage index | O(n) 
| Construction phrases | O(n × p) | O(total phrases) |
| Reconstruction texte | O(w) | O(fichier) |

