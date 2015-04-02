#ifndef _BOARD_CLIENT_H
#   define _BOARD_CLIENT_H

#include "board.h"
#include <arpa/inet.h>        /*  inet (3) funtions         */

/**
 * Type of the handle to a board;
 */
struct board_t {
    int       socket;               /*  connection socket         */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
};

/**
 * Connect to the board locate at `host`:`port`
 *
 * return 0 if ok, -1 if there is a problem
 */
int bd_connect(struct board_t *board, const char* host, int port);

/**
 * Disconnect from the board
 */
int bd_disconnect(const struct board_t *board);

/**
 * Print `str` at `line`.
 *
 * Return `> 0` if the query completed, 0 if the query was rejected (use bd_error to get the message), <0 in case of error (device disconnected ...)
 */
int bd_send_line(const struct board_t *board, int line, const char* str);

/**
 * Change the state of all buttons. `button` is a mask with all status.
 *
 * Return (see bd_send_line)
 */
int bd_send_button_states(const struct board_t *board, int button);

/**
 * Read the states of the buttons. Note, all button pressed since last call are returned.
 *
 * Return a bitmask of all button state, or -1 if error
 */
int bd_read_button_state(const struct board_t *board);

/**
 * Read a key pressed. Keys are buffered and more than one call may be needed.
 *
 * Return -1 if error, 0 if no key where pressed, the ascii code of the key pressed otherwise.
 */
int bd_read_key(const struct board_t *board);


/**
 * Return the last error message.
 * This is valid only until the next bd_call
 */
const char* bd_error();

/**
 * Send a query to the board (via buffer) and collect the result (via buffer)
 *
 * return -1 if error, 0 if eof, the size of the buffer otherwise
 */
int bd_send_command(const struct board_t *board, void *buffer, int datalen, int bufflen);

#endif
