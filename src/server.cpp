#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>

#include "server.h"
#include "settings.h"
/**
 * @brief Construct a new Server:: Server object is created with bound address 127.0.0.1:8080
 *
 */
Server::Server()
{
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("Server: Socket Failed: ");

    // Core Address setup
    struct sockaddr_in serverAddr;

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVER_PORT);

    // Only need for consecutive use to prevent lingering in the background on sig interp
    struct timeval tv;
    tv.tv_sec = SERVER_TIME_OUT_SEC;
    tv.tv_usec = 0;
    int optval = 1;
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEPORT, (const void *)&optval, sizeof(int));
    setsockopt(serverfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Bind address for core
    if (bind(serverfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        error("Server: Bind Failed: ");

    server_debug_print("%s", "Socket successfully binded..\n");
}
/**
 * @brief Destroy the Server:: Server object
 *
 */
Server::~Server()
{
    close(serverfd);
}
/**
 * @brief Listens for a new connection and accepts it, putting the new connection into Server.clientfd
 *
 */
void Server::connection_listen()
{
    // ClientAddr in .h
    // ClientAddrLen in .h
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;

    clientAddrLen = sizeof(clientAddr);

    server_debug_print("%s", "Server listening..\n");

    if (listen(serverfd, 1) < 0)
        error("Server: connectionlisten");

    if ((clientfd = accept(serverfd, (struct sockaddr *)&clientAddr, &clientAddrLen)) < 0)
        error("Server: accept sockfd <= 0: ");
}
void Server::connection_accept()
{
}
void Server::connection_read()
{
    char buffer[SERVER_BUFFER_MAX];
    int n;
    n = read(clientfd, buffer, SERVER_BUFFER_MAX);
    buffer[n] = '\0';
    server_debug_print("%s", buffer);

    std::string buf = std::string(server_file_read("src/res/hello.html"));

    sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n%s", buf.length(), buf.data());
    write(clientfd, buffer, strlen(buffer));
}

/**
 * @brief Reads whole file into buffer
 *
 * @param fileLocation Location of file
 * @return char* Return buffer of the whole file
 */
std::string server_file_read(std::string fileLocation)
{
    FILE *file = fopen(fileLocation.data(), "rb");
    if (file == NULL)
        error("Server: File ERROR:");

    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // char *buffer = (char *)malloc(sizeof(char) * fileSize + 1);
    char buffer[fileSize + 1];
    fread(buffer, sizeof(char), fileSize, file);
    fclose(file);

    buffer[fileSize] = '\0';
    return buffer;
}

Connection::Connection(int _clientfd)
{
    clientfd = _clientfd;
}

Connection::~Connection()
{
}

void Connection::conn_recvieve()
{
}

/**
 * @brief Define DEBUG to allow for printing in consol.
 *
 * @param frmt takes frmt string like printf
 * @param debugMessage takes any string you would like to print
 */
void server_debug_print(std::string frmt, std::string debugMessage)
{
#ifdef DEBUG
    printf(frmt.data(), debugMessage.data());
#endif
}
/**
 * @brief Server object error handling. Prints out perror.
 *
 * @param errorMessage Usually signify a location in the code.
 */
void error(std::string errorMessage)
{
    std::perror(errorMessage.data());
    exit(EXIT_FAILURE);
}