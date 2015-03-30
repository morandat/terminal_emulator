#include <iostream>
#include <string.h>

#define LISTENQ    (1024)   /*  Backlog for listen()   */

class Socket{
private:
	int list_s;
    int conn_s;

public:
    Socket(int port);
    Socket(std::string addr);

	bool SetSocketBlockingEnabled(int fd, bool blocking);

    bool isConnected();
    std::string Receive();
    bool Send(std::string text);
    bool Send(char *text, int len);
    bool Send(char	text);
};
