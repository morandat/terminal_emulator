#include <stdio.h> // printf
#include <stdlib.h> // perror/EXIT_SUCCESS/...
#include <unistd.h> // Sleep

#include <board_client.h> // Pour les fonctions du simulateur

const char *star_wars[] = {
  "La République Galactique est en pleine ébullition.",
  "La taxation des routes commerciales reliant les systèmes",
  "éloignés provoque la discorde.",
  "",
  "Pour régler la question, la cupide Fédération du Commerce",
  "et ses redoutables vaisseaux de guerre imposent un blocus",
  "à la petite planète Naboo.",
  "",
  "Face à ce dangereux engrenage, alors que le Congrès de la",
  "République s’enlise dans des débats sans fin, le Chancelier",
  "Suprême charge en secret deux Chevaliers Jedi, gardiens de",
  "la paix et de la justice dans la galaxie, de résoudre le",
  "conflit...",
  NULL };

const char *smiley[] = {
          "                          oooo$$$$$$$$$$$$oooo",
          "                      oo$$$$$$$$$$$$$$$$$$$$$$$$o",
          "                   oo$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$o         o$   $$ o$",
          "   o $ oo        o$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$o       $$ $$ $$o$",
          "oo $ $ '$      o$$$$$$$$$    $$$$$$$$$$$$$    $$$$$$$$$o       $$$o$$o$",
          "'$$$$$$o$     o$$$$$$$$$      $$$$$$$$$$$      $$$$$$$$$$o    $$$$$$$$",
          "  $$$$$$$    $$$$$$$$$$$      $$$$$$$$$$$      $$$$$$$$$$$$$$$$$$$$$$$",
          "  $$$$$$$$$$$$$$$$$$$$$$$    $$$$$$$$$$$$$    $$$$$$$$$$$$$$  '''$$$",
          "   '$$$''''$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$     '$$$",
          "    $$$   o$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$     '$$$o",
          "   o$$'   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$       $$$o",
          "   $$$    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" "$$$$$$ooooo$$$$o",
          "  o$$$oooo$$$$$  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$   o$$$$$$$$$$$$$$$$$",
          "  $$$$$$$$'$$$$   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$     $$$$''''''''",
          " ''''       $$$$    '$$$$$$$$$$$$$$$$$$$$$$$$$$$$'      o$$$",
          "            '$$$o     '''$$$$$$$$$$$$$$$$$$'$$'         $$$",
          "              $$$o          '$$''$$$$$$''''           o$$$",
          "               $$$$o                                o$$$'",
          "                '$$$$o      o$$$$$$o'$$$$o        o$$$$",
          "                  '$$$$$oo     ""$$$$o$$$$$o   o$$$$''",
          "                     ''$$$$$oooo  '$$$o$$$$$$$$$'''",
          "                        ''$$$$$$$oo $$$$$$$$$$",
          "                                ''''$$$$$$$$$$$",
          "                                    $$$$$$$$$$$$",
          "                                     $$$$$$$$$$'",
          "                                      '$$$''  ",
          NULL};

void exit_if(int cond, const char *msg) {
  if (cond) {
    perror(msg);
    exit(EXIT_FAILURE);
  }
}

void clear_screen(struct board_t *board) {
  for(int i = 0; i < SCREEN_LINES; i ++)
    bd_send_line(board, i, "");
}

void scroll_screen(struct board_t *board, const char *text[]) {
    clear_screen(board);
    for (int i = 0; i < SCREEN_LINES; i ++)
      bd_send_line(board, SCREEN_LINES - i - 1, text[1]);
}

void print_screen(struct board_t *board, const char *text[]) {
  int i = 0;
  for (i = 0; smiley[i] && i < SCREEN_LINES; i ++)
    bd_send_line(board, i, smiley[i]);
  while (i < SCREEN_LINES)
    bd_send_line(board, i, "");
}

int main(int argc, char *argv[]) {
  struct board_t board;
  int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;
  int res = bd_connect(&board, DEFAULT_HOST, port); // Se connecte au simulateur
  exit_if(res < 0, "Connection impossible");

  bd_send_button_states(&board, 0x82); // Allume le boutton 8 et 2
  int loop = 0;
  while (!(bd_read_button_state(&board) & 0x80)) { // Tant que le 8 boutton n'est pas appuyé
    if (loop++ % 2) // On affiche un joli smiley ou pas ...
      clear_screen(&board);
    else
      print_screen(&board, smiley);
    sleep(1);
  }

  bd_send_line(&board, 11, "Veuillez saisir votre pin"); // Affiche une invite
  char line[] = "                                                                                ";
  int pin = 0;
  int digit = 4;
  while (digit) {
    int k = bd_read_key(&board); // On lit une touche
    if (k >= '0' && k <= '9') { // Si c'est un chiffre
      pin = pin * 10 + (k - '0'); // on calcule la valeur du pin (int)
      line[44 + 8 - 2 * digit] = '*'; // et on rajoute une '*'
      digit --;
    }
    bd_send_line(&board, 13, line); // et on affiche les '*'
    usleep(100);
  }
  printf("Le pin est %d\n", pin);
  bd_disconnect(&board); // On se déconnecte du simulateur

  return EXIT_SUCCESS;
}

