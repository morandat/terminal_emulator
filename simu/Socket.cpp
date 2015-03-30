#include <unistd.h>
#include <sys/types.h>        /*  socket types              */
#include <sys/socket.h>       /*  socket definitions        */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <fcntl.h>

#include "Socket.h"

Socket::Socket(int port)
{
    struct    sockaddr_in servaddr;
    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	    fprintf(stderr, "Error creating listening socket.\n");
    	exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);


    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
    fprintf(stderr, "Error calling bind()\n");
    exit(EXIT_FAILURE);
    }


    if ( listen(list_s, LISTENQ) < 0 ) {
        fprintf(stderr, "Error calling listen()\n");
        exit(EXIT_FAILURE);
    }

    /*  Wait for a connection, then accept() it  */
    printf("(II) Waiting for client connection (%d)\n", port);
    if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 ) {
        fprintf(stderr, "Error calling accept()\n");
        exit(EXIT_FAILURE);
    }

    printf("(II) Client connection accepted");
    SetSocketBlockingEnabled(conn_s, false);
}


/** Returns true on success, or false if there was an error */
bool Socket::SetSocketBlockingEnabled(int fd, bool blocking)
{
   if (fd < 0) return false;

#ifdef WIN32
   unsigned long mode = blocking ? 0 : 1;
   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags < 0) return false;
   flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
   return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
}

bool Socket::isConnected()
{
	return true;
}

std::string Socket::Receive( )
{
	char buffer[1024];
	bzero(buffer, 1024);
    int rSize = read(conn_s, buffer, 1024);
    std::string r = buffer;
    return r;
}


bool Socket::Send(std::string text){
	int wSize = write(conn_s, text.c_str(), text.length()+1);
	return true;
}

bool Socket::Send(char *text, int len){
	return write(conn_s, text, len);
}

bool Socket::Send(char text){
	int wSize = write(conn_s, &text, 1);
	return true;
}
