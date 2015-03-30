#include <unistd.h>           /*  misc. UNIX functions      */
#include <sys/types.h>        /*  socket types              */
#include <sys/socket.h>       /*  socket definitions        */
#include <arpa/inet.h>        /*  inet (3) funtions         */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


#define MAX_LINE           (1000)

#define DEFAULT_PORT        2002
#define DEFAULT_HOST        "127.0.0.1"

#define SCREEN_LINES    26
#define SCREEN_COLUMNS  80

// For query and replies
#define BUFFER_SIZE         1024

#define OK_STRING           "#OK"
#define KO_STRING           "#KO"

struct device_t {
  int       socket;               /*  connection socket         */
  struct    sockaddr_in servaddr;  /*  socket address structure  */
};

/**
 * Se connecter au simulateur sur l'hote/port spécifié
 *
 * Retourne -1 en cas d'erreur (0 si tout est ok)
 */
int connect(struct device_t *board, const char* host, int port) {
  if ((board->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    return -1;
  }

  if (inet_aton(host, &board->servaddr.sin_addr) <= 0 ) {
    return -1;
  }
  board->servaddr.sin_family = AF_INET;
  board->servaddr.sin_port = htons(port);

  if ( connect(board->socket, (struct sockaddr *) &board->servaddr, sizeof(board->servaddr) ) < 0 ) {
    return -1;
  }
  return 0;
}


static int is_ok(const char *buffer) {
  if (strncmp(OK_STRING, buffer, 3) == 0)
    return 1;
  if (strncmp(KO_STRING, buffer, 3) == 0)
    return 0;
  return -1;
}

int send_command(const struct device_t *board, void *buffer, int datalen, int bufflen) {
  if (write(board->socket, buffer, datalen) < datalen)
    return -1;
  return read(board->socket, buffer, bufflen);
}

static int mysnprintf(char *str, size_t size, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(str, size, format, ap) + 1;
  va_end(ap);

  if (len > size) {
    len = size;
    str[len - 1] = 0; // this is useless I think ... but we are never sure enough
  }
  return len;
}

/**
 *
 * return 1 if the request was completed, 0 if not, -1 if error
 */
int send_text_line(const struct device_t *board, int line, const char* str) {
  char buffer[BUFFER_SIZE];
  int len = mysnprintf(buffer, BUFFER_SIZE, "# L %d %s", line, str);
  if (send_command(board, buffer, len, BUFFER_SIZE) > 0 )
    return is_ok(buffer);
  return -1;
}

int send_button_color(const struct device_t *board, int button, int color) {
  char buffer[BUFFER_SIZE];
  int len = mysnprintf(buffer, BUFFER_SIZE, "# C %d %d", button, color);
  if (send_command(board, buffer, len, BUFFER_SIZE) > 0)
    return is_ok(buffer);
  return -1;
}

int read_key(const struct device_t *board) {
  char buffer[BUFFER_SIZE];
  int len = mysnprintf(buffer, BUFFER_SIZE, "# K");
  if (send_command(board, buffer, len, BUFFER_SIZE) > 0 && is_ok(buffer)) {
    if (buffer[3] != ':')
      return 0;
    return strtol(buffer + 4, NULL, 10);
  }
  return -1;
}

int read_button(const struct device_t *board) {
  char buffer[BUFFER_SIZE];
  int len = mysnprintf(buffer, BUFFER_SIZE, "# B");
  if (send_command(board, buffer, len, BUFFER_SIZE) > 0 && is_ok(buffer)) {
    if (buffer[3] != ':')
      return 0;
    return strtol(buffer + 4, NULL, 10);
  }
  return -1;
}

void exit_if(int cond, const char *msg) {
  if (cond) {
    perror(msg);
    exit(EXIT_FAILURE);
  }
}

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


void clear_screen(struct device_t *board) {
  for(int i = 0; i < SCREEN_LINES; i ++)
    send_text_line(board, i + 1, "");
}

void print_screen(struct device_t *board, const char *text[]) {
  int i = 0;
  for (i = 0; smiley[i] && i < SCREEN_LINES; i ++)
    send_text_line(board, i + 1, smiley[i]);
  while ( i < SCREEN_LINES)
    send_text_line(board, i + 1, "");
}

int main(int argc, char *argv[]) {
  struct device_t board;
  int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;
  exit_if(connect(&board, DEFAULT_HOST, port) < 0, "Connection impossible");

  send_button_color(&board, 8, 1);
  int loop = 10;
  while (loop -- && read_button(&board) != 8) {
    if (loop % 2)
      print_screen(&board, smiley);
    else
      clear_screen(&board);
    sleep(1);
  }

  send_text_line(&board, 11, "Veuillez saisir votre pin");
  char line[] = "                                                                                ";
  int pin = 0;
  int digit = 4;
  while (digit) {
    int k = read_key(&board);
    if (k >= '0' && k <= '9') {
      pin = pin * 10 + (k - '0');
      line[44 + 8 - 2 * digit] = '*';
      digit --;
    }
    send_text_line(&board, 13, line);
    usleep(100);
  }

  return EXIT_SUCCESS;
}

