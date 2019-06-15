Projet 'Bureau d'étude Réseaux', 3e année INSA Toulouse, Pré-orientation MIC.

Suite logique du projet 'Applications Internet et Programmation Socket ' : [AIPS_tp](https://github.com/KilianDesportes/AIPS_tp "AIPS_tp")


DELIOT Maxence

DESPORTES Kilian


3 MIC
Groupe C
2018-2019.



Developpement d'uun protocole (MICTCP) mettant en œuvre un mécanisme de reprise des pertes « à fiabilité partielle » :
* tolérant un % de pertes « acceptables » par l’application
* mais conduisant à une meilleure « fluidité » de l’application en cas de pertes


Démontrer au final le bien fondé de MIC-TCP comparativement à TCP lorsque le réseau génère des pertes
* mais conduisant à une meilleure « fluidité » de l’application en cas de pertes


4 versions sont disponibles: 
* v1 : Phase de transfert de données sans garantie de fiabilité.
* v2 : Garantie de fiabilité totale via un mécanisme de reprise des pertes de type « Stop and Wait ».
* v3 : Garantie de fiabilité partielle « statique » via un mécanisme de reprise des pertes de type « Stop and Wait » à fiabilité partielle « pré câblée », i.e. dont le % de pertes admissibles est défini de façon statique.
* v4 : Phase d’établissement de connexion + Garantie de fiabilité partielle via un mécanisme de reprise des pertes de type « Stop and Wait » dont le % de pertes admissibles est à présent négocié durant la phase d’établissement de connexion.

Génération de l'exécutable via un Makefile : `$ make`


Test du protocole via plusieurs exécutables fournis, *tsock_texte* et *tsock_video*.


Note obtenue : 16.5/20
