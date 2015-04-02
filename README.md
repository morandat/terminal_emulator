## Compilez le simulateur

Le Makefile est censé bien faire son travail, vous pouvez tout compiler depuis ici :

```
make -C board
```

## Compilez l'exemple

A nouveau le Makefile est bien fait, complilez l'exemple depuis ici :

```
make -C client example
```

puis lancez le :

```
# On lance d'abord le simulateur
./board/board &
# Puis le programme d'exemple
./client/example
```

## Préparez votre projet

Votre code source doit se situer dans le répertoire `client`. 
Vous pouvez vous insiprer du code de `example.c` pour réaliser votre projet.
Pensez à adapter le `Makefile` fourni (normalement vous n'aurez que deux lignes à toucher).
Les fichiers à regarder **impérativement** sont :

`board/board.h`
: Il contient les constantes *magiques* du simulateur

`board/board_client.h`
: Il contient les prototypes des fonctions que vous pourrez/devrez utiliser

`board/board_client.c`
: Seulement pour les plus curieux, il contient la code source des fonctions de communications avec le serveur.

### Dépendences

MacOSX
: - Installez XCode (appstore) puis les outils en ligne de commandes
  - ` xcode-select --install` (ou cherchez dans les préférences de XCode)
  - Si vous utilisez homerew (<http://brew.sh/>) : `brew install homebrew/science/opencv`
  - Si vous utilisez macport (<https://www.macports.org/>) : `sudo port install opencv`
  - (Si vous n'en avez aucun des deux, optez pour homebrew)

Linux (Débian/Ubuntu)
: Installez les paquets `build-essential` et `libopencv-dev`  (`sudo apt-get install build-essential libopencv-dev`)

Windows
: Aucune idée
