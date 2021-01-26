# Multi Core Unix wc
_Implémentation de Thomas MICHEL_

## Indications nécessaires
Tout est indiqué dans les commentaires des fichiers. 

Cette implémentation utilise 4 threads.

Ce programme peut être plus lent que wc pour des petits fichiers mais devient plus rapide pour des gros fichiers. En testant sur un fichier de 1Go (obtenu par la commande donnée dans l'énoncé) la durée d'exécution à l'aide de la commande time, j'ai obtenu:
- 3 secondes pour mon mwc
- 20 secondes pour la commande wc classique
- 2min50 pour le programme fourni dans l'archive de départ

Je pense donc avoir réussi le défi.

------------
## Fichiers
- mwc
- main.c
- counter.c -> Implémente les fonctions de comptage de mots et de lignes
- counter.h
- Makefile
- README.md
- tests/..