#include <unistd.h>
#include <sys/types.h>        /*  socket types              */
#include <sys/socket.h>       /*  socket definitions        */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <fcntl.h>
#include <errno.h>

#include "Socket.h"

Socket::Socket()
{
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    throw "Error creating socket().";
}

void Socket::close() {
  if (fd != -1)
    ::close(fd);
  fd = -1;
}

Socket::Socket(int sock): fd(sock) {
}

Socket::~Socket() {
  close(); // ensure socket is closed
}

ClientSocket::ClientSocket(std::string addr): Socket() {
  if (!addr.empty())
    connect(addr);
}

void ClientSocket::connect(std::string addr) {
  throw "Cannot connect()";
}

void Socket::bind(int port, long s_addr, short in_family) {
    struct    sockaddr_in servaddr;
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);
    bind((struct sockaddr *) &servaddr, sizeof(servaddr));
}

void Socket::bind(struct sockaddr* addr, size_t sizeOfSockAddr) {
  if (::bind(fd, addr, sizeOfSockAddr) < 0)
    throw "Cannot bind() socket";
}

ServerSocket::ServerSocket(int port, int queue_size) : Socket() {
  bind(port);

  if (listen(fd, queue_size) < 0)
    throw "Error listen()ing on socket";
}

Socket& ServerSocket::accept() {
    int nsocket;
    if ( (nsocket = ::accept(fd, NULL, NULL) ) < 0 )
        throw "Can't accept() client";

    Socket* client = new Socket(nsocket);
    client->setBlockingState(false);
    return *client;
}

/** Returns true on success, or false if there was an error */
bool Socket::setBlockingState(bool blocking) {
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

std::string Socket::receive( )
{
    std::string r;
	char buffer[1024];
    int rSize = receive(buffer, 1024 - 1);
    if (rSize > 0 && rSize < 1024) { // the second part is currently always true by construction
      buffer[rSize] = 0;
      r = buffer;
    }
    return r;
}

// This one is not really robust (e.g., size > 1024)
int Socket::receive(int size) {
	char buffer[size];
    return receive(buffer, size);
}

int Socket::receive(void *buffer, int size) {
    int rSize = read(fd, buffer, size);
    if (rSize < 0 && errno != EAGAIN)
      throw "read() error"; // return -1 ?
    return rSize;
}

bool Socket::send(std::string text){
  int len = text.length() + 1;
  const char *buffer = text.c_str();
  return send(buffer, len);
}

bool Socket::send(const char *text, int len){
  while (len) {
    int wSize = write(fd, text, len);
    if (wSize < 0) {
      if (errno != EAGAIN)
        throw "write() error";
    } else if (wSize == 0) {
      return false;
    } else {
      len -= wSize;
      text += wSize;
    }
  }
  return true;
}

bool Socket::send(char text){
  return send(&text, 1);
}
