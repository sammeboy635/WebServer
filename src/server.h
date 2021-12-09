#ifndef __SERVER_H
#define __SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

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

std::string server_file_read(std::string fileLocation);
void server_debug_print(std::string frmt, std::string debugMessage);
void error(std::string errorMessage);

class Server
{
private:
    /* data */
public:
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;
    int serverfd, clientfd;
    fd_set listenSet;
    Server();
    ~Server();
    void connection_listen();
    void connection_read();
    void connection_accept();
};

class Connection
{
private:
    int clientfd;

public:
    Connection(int clientfd);
    ~Connection();
    void conn_recvieve();
};

#endif