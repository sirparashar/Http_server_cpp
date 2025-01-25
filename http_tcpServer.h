#ifndef INCLUDE_HTTP_TCPSERVER
#define INCLUDE_HTTP_TCPSERVER

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
using namespace std;
namespace http
{
    class tcpServer
    {
    public:
        tcpServer(string ip_addr, int port);
        ~tcpServer();
        void startlistening();

    private:
        string m_ip_addr;
        int m_port;
        int m_socket;
        int m_new_socket;
        long m_incomingMsg;
        struct sockaddr_in m_socket_address;
        unsigned int m_addr_length;
        string m_serverMsg;

        int startServer();
        void closeServer();
        void acceptConnection(int &new_socket);
        string buildResponse();
        void sendResponse();
        void handleClient(int client_socket);
        void workerThread(int thread_id);
    };
}

#endif // INCLUDE_HTTP_TCPSERVER
