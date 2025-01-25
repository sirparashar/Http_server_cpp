#include "http_tcpServer.h"

int main()
{
    using namespace http;
    tcpServer server = tcpServer("127.0.0.1", 8080);
    server.startlistening();
}