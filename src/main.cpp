// Started @10:27am
// Ended @2:35pm

#include <unistd.h>
#include <stdio.h>
#include <cstring>

#include "settings.h"
#include "server.h"
#define MAX 1024
void test_server()
{
    Server webserver;
    webserver.connection_listen();
    webserver.connection_read();
}

int main(int argc, char const *argv[])
{
    test_server();
    exit(EXIT_SUCCESS);
}
