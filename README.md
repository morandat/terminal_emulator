## Touches utiles

> Assurez vous que la fênetre du simulateur ait le focus.

- **escape** : Pour quitter le simulateur
- **F1**..**F8**: bouton 1 à 8
- **F9**..**F12**: bouton 5 à 8 (oui, oui, il y'a bien deux moyens d'appuyer sur le bouton 5)

## Compilez le simulateur

Le Makefile est censé bien faire son travail, vous pouvez tout compiler depuis ici :

```
make -C board
```

## Compilez l'exemple

A nouveau le Makefile est bien fait, compilez l'exemple depuis ici :

```
make -C client example
```

puis lancez le :

```
# On lance d'abord le simulateur
./board/board &
# Puis le programme d'exemple
./client/example

## Ou faites le tout en une seule ligne ...
./board/board& sleep 1; ./client/example
```

> **Attention** depuis les machines pédagogiques de l'enseirb, il faut rajouter
> `EIRB=1` dans le make du répertoire board, i.e.,
> `make -C board EIRB=1`

## Préparez votre projet

Votre code source doit se situer dans le répertoire `client`. 
Vous pouvez vous inspirer du code de `example.c` pour réaliser votre projet.
Pensez à adapter le `Makefile` fourni (normalement vous n'aurez que deux lignes à toucher).
Les fichiers à regarder **impérativement** sont :

`board/board.h`
: Il contient les constantes *magiques* du simulateur.

`board/board_client.h`
: Il contient les prototypes des fonctions que vous pourrez/devrez utiliser.

`board/board_client.c`
: Seulement pour les plus curieux, il contient la code source des fonctions de communications avec le serveur.

### Dépendences

MacOSX
: - Installez XCode (appstore) puis les outils en ligne de commandes
  - ` xcode-select --install` (ou cherchez dans les préférences de XCode)
  - Si vous utilisez homebrew (<http://brew.sh/>) : `brew install homebrew/science/opencv`
  - Si vous utilisez macport (<https://www.macports.org/>) : `sudo port install opencv`
  - (Si vous n'en avez aucun des deux, optez pour homebrew)

Linux (Débian/Ubuntu)
: Installez les paquets `build-essential` et `libopencv-dev`  (`sudo apt-get install build-essential libopencv-dev`)

Windows
: Aucune idée
