#include <unistd.h>           /*  misc. UNIX functions      */
#include <sys/types.h>        /*  socket types              */
#include <sys/socket.h>       /*  socket definitions        */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "board_client.h"

// For query and replies
#define BUFFER_SIZE         1024

#define NO_MESSAGE "There was an error, but no message were provided by the board"
#define NO_ERROR "Success"

static char last_error_buffer[80];

static const char *last_error = NO_ERROR;

/**
 * Se connecter au simulateur sur l'hote/port spécifié
 *
 * Retourne -1 en cas d'erreur (0 si tout est ok)
 */
int bd_connect(struct board_t *board, const char* host, int port) {
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
    if (strncmp(KO_STRING, buffer, 3) == 0) {
        last_error = (buffer[3] == ':') ?  strncpy(last_error_buffer, buffer + 4, sizeof(last_error_buffer) - 1) : NO_MESSAGE;
        return 0;
    }
    return -1;
}

int bd_send_command(const struct board_t *board, void *buffer, int datalen, int bufflen) {
    last_error = NO_ERROR;
    if (write(board->socket, buffer, datalen) < datalen)
        return -1;
    return read(board->socket, buffer, bufflen);
}

static int mysnprintf(char *str, size_t size, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int len = vsnprintf(str, size, format, ap);
    va_end(ap);

    if (len > size) {
        len = size;
        str[len - 1] = 0; // this is useless I think ... but we are never sure enough
    }
    return len;
}

int bd_send_line(const struct board_t *board, int line, const char* str) {
    char buffer[BUFFER_SIZE];
    int len = mysnprintf(buffer, BUFFER_SIZE, "#L%2.2x:%s" CRLF, line, str);
    if (bd_send_command(board, buffer, len, BUFFER_SIZE) > 0 )
        return is_ok(buffer);
    return -1;
}

int bd_send_button_states(const struct board_t *board, int button) {
    char buffer[BUFFER_SIZE];
    int len = mysnprintf(buffer, BUFFER_SIZE, "#C%2.2x" CRLF, button & 0xff);
    if (bd_send_command(board, buffer, len, BUFFER_SIZE) > 0)
        return is_ok(buffer);
    return -1;
}

int bd_send_button_text(const struct board_t *board, int button, const char *text) {
    char buffer[BUFFER_SIZE];
    int len = mysnprintf(buffer, BUFFER_SIZE, "#T%1.1x:%s" CRLF, button & 0xf, text);
    if (bd_send_command(board, buffer, len, BUFFER_SIZE) > 0)
        return is_ok(buffer);
    return -1;
}

int bd_read_key(const struct board_t *board) {
    char buffer[BUFFER_SIZE];
    int len = mysnprintf(buffer, BUFFER_SIZE, "#K" CRLF);
    if (bd_send_command(board, buffer, len, BUFFER_SIZE) > 0 && is_ok(buffer)) {
        if (buffer[3] != ':')
            return 0;
        return strtol(buffer + 4, NULL, 16);
    }
    return -1;
}

int bd_read_button_state(const struct board_t *board) {
    char buffer[BUFFER_SIZE];
    int len = mysnprintf(buffer, BUFFER_SIZE, "#B" CRLF);
    if (bd_send_command(board, buffer, len, BUFFER_SIZE) > 0 && is_ok(buffer)) {
        if (buffer[3] != ':')
            return 0;
        return strtol(buffer + 4, NULL, 16);
    }
    return -1;
}

int bd_disconnect(const struct board_t *board) {
    char buffer[BUFFER_SIZE];
    int len = mysnprintf(buffer, BUFFER_SIZE, "#Q" CRLF);
    int res = bd_send_command(board, buffer, len, BUFFER_SIZE);
    close(board->socket);
    return res == 0;
}

const char* bd_error() {
    return last_error;
}
