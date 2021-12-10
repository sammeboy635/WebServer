#ifndef __SERVER_H
#define __SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

#include "settings.h"

enum HTTPMethods
{
    CONNECT,
    DELETE,
    GET,
    HEAD,
    OPTIONS,
    PATCH,
    POST,
    PUT,
    TRACE
};

struct Set
{
    int maxfd;
    fd_set fds;
    int sockfdslen;
    int sockfds[SERVER_MAX_CONNECTIONS];
};

std::string server_file_read(std::string fileLocation);
void server_debug_print(std::string frmt, std::string debugMessage);
void error(std::string errorMessage);

class Server
{
private:
    int serverfd, clientfd;
    Set set;

    void connection_read(int clientsfds);

public:
    Server();
    ~Server();
    void connection_listen();
    void connection_accept(int _serverfd);
    void connection_set_loop();
    void connection_read();
};

#endif