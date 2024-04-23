# Jeu de l'Othello

Ce programme implémente le jeu de l'Othello avec une interface graphique développé avec la bibliothèque SFML. Il propose 3 modes de jeu dont 2 qui font intervenir un réseau de neurones que l'on peut entraîner au fur et à mesure à l'aide de la console. Ce projet est un prétexte pour jouer avec les IAs et essayer de découvrir leur fonctionnement sans avoir recours à des bibliothèques qui implémente déjà toute la logique des IAs.

## Modes de jeu

- Mode 1v1 : 

Mode multijoueur : 2 joueurs s'affrontent.

- Mode IA :

Mode solo : un joueur affronte un réseau de neurones artificiels.

- Mode Test :

Mode zéro joueur : Permet de regarder l'IA jouer contre un bot jouant le coup qui retourne le plus de cases.

## Commandes

Pour changer de mode : utiliser la touche `M`

Pour relancer une nouvelle partie : utiliser la touche `R`

Toutes les paramètres peuvent être modifié directemetn dans le code (voir les macros)

## IA

L'interface de commande permet d'entraîner l'IA en sélectionnant le nombre de générations et les moyens de s'entraîner. Tapper `help` pour avoir plus d'informations.
