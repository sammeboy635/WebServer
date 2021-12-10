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
#include <sys/select.h>

#include "server.h"
#include "settings.h"
/**
 * @brief Construct a new Server:: Server object is created with bound address 127.0.0.1:8080
 *
 */
Server::Server()
{
    // Init set
    set.maxfd = 0;
    set.sockfdslen = 0;
    memset(set.sockfds, 0, sizeof(set.sockfds));
    FD_ZERO(&set.fds);

    // Init server socket
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

    // Add server to list of reading
    FD_SET(serverfd, &set.fds);
    set.maxfd = serverfd;

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
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;

    clientAddrLen = sizeof(clientAddr);

    server_debug_print("%s", "Server listening..\n");

    if (listen(serverfd, 1) < 0)
        error("Server: connectionlisten");

    if ((clientfd = accept(serverfd, (struct sockaddr *)&clientAddr, &clientAddrLen)) < 0)
        error("Server: accept sockfd <= 0: ");
}
void Server::connection_accept(int _serverfd)
{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;

    clientAddrLen = sizeof(clientAddr);

    if (set.sockfdslen == SERVER_MAX_CONNECTIONS) // Max
        return;
    if (listen(serverfd, 1) < 0)
        error("Server: connectionlisten");
    if ((clientfd = accept(_serverfd, (struct sockaddr *)&clientAddr, &clientAddrLen)) < 0)
        error("Server: accept sockfd <= 0: ");

    // Needed to reduce Set iterations
    if (set.maxfd < clientfd)
        set.maxfd = clientfd;

    // Add to the list of fds
    FD_SET(clientfd, &set.fds);
    // Add to the list of set sockfds and increase the length
    set.sockfds[set.sockfdslen] = clientfd;
    set.sockfdslen++;

    return;
}
/**
 * @brief Handles sets of tcp
 *
 */
void Server::connection_set_loop()
{
    fd_set readSetfds;
    FD_ZERO(&readSetfds);

    while (true)
    {

        printf("Waiting:\n");
        readSetfds = set.fds;
        // Selects the ready to read tcp fds
        int nready = 0;
        if ((nready = select(set.maxfd + 1, &readSetfds, NULL, NULL, NULL)) < 0)
            error("Server: set size was -1: ");

        if (nready == 0)
            continue;

        // if it is serverfd then its a new connection to server;
        if (FD_ISSET(serverfd, &readSetfds))
        {
            connection_accept(serverfd);
            continue;
        }
        // Iterate over the available clientfds
        for (int i = 0; i <= set.sockfdslen; i++)
        {
            // Check if the sockfd is in the readSetfds;
            if (FD_ISSET(set.sockfds[i], &readSetfds))
                connection_read(set.sockfds[i]);
        }
    }
}
/**
 * @brief handle the read
 *
 */
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
void Server::connection_read(int _clientfd)
{
    char buffer[SERVER_BUFFER_MAX];
    int n;
    n = read(_clientfd, buffer, SERVER_BUFFER_MAX);
    buffer[n] = '\0';
    server_debug_print("%s", buffer);

    std::string buf = std::string(server_file_read("src/res/hello.html"));

    sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n%s", buf.length(), buf.data());
    write(_clientfd, buffer, strlen(buffer));
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
        error("Server: file error:");

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