// Started @10:27am
// Ended @2:35pm

#include <unistd.h>
#include <stdio.h>
#include <cstring>

#include "settings.h"
#include "server.h"
#define MAX 1024
/**
 * @brief Tests the listen read, and write on tcp;
 *
 */
void test_server()
{
    Server webserver;
    webserver.connection_listen();
    webserver.connection_read();
}
/**
 * @brief Testing the server with accept, read, and write;
 *
 */
void test_server_set()
{
    Server webserver;
    webserver.connection_set_loop();
}

int main(int argc, char const *argv[])
{
    test_server_set();
    exit(EXIT_SUCCESS);
}
