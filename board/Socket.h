#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <string>

#define LISTENQ    1024   /*  Backlog for listen()   */

class Socket{
protected:
    int fd;
    Socket();
    Socket(int sockid);
    friend class ServerSocket;
public:
    virtual ~Socket();

    void bind(int port, long s_addr = INADDR_ANY, short in_family = AF_INET);
    void bind(struct sockaddr* addr, size_t sizeOfSockAddr);

	bool setBlockingState(bool blocking);

    bool isConnected();

    // Does receive and send method have any sense on a server socket ?
    std::string receive();
    int receive(int size);
    int receive(void *buffer, int size);

    bool send(char text);
    bool send(const char *text, int len);
    bool send(std::string text);

    void close();
};

class ClientSocket: Socket {
public:
    ClientSocket(std::string addr);
    void connect(std::string addr);
};

class ServerSocket: public Socket {
public:
    ServerSocket(int port, int queue_size = LISTENQ);
    Socket& accept();
};
