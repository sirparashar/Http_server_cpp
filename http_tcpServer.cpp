#include "http_tcpServer.h"
#include <iostream>
#include <sys/socket.h>
#include <sstream>
#include <unistd.h>

const int BUFFER_SIZE = 30720;
void log(const std::string &msg)
{
    std::cout << msg << std::endl;
}

void exitWithError(const std::string &error)
{
    std::cerr << error << std::endl;
    exit(1);
}

namespace http
{
    tcpServer::tcpServer(std::string ip_addr, int port) : m_ip_addr(ip_addr), m_port(port), m_socket(),
                                                          m_new_socket(), m_incomingMsg(), m_socket_address(),
                                                          m_addr_length(sizeof(m_socket_address)), m_serverMsg(buildResponse())
    {
        m_socket_address.sin_family = AF_INET;
        m_socket_address.sin_port = htons(m_port);
        m_socket_address.sin_addr.s_addr = inet_addr(m_ip_addr.c_str());
        startServer();
    }
    tcpServer::~tcpServer()
    {
        closeServer();
    }

    void tcpServer::handleClient(int client_socket)
    {
        char buffer[BUFFER_SIZE] = {0};
        int bytesReceived = read(client_socket, buffer, BUFFER_SIZE);
        if (bytesReceived < 0)
        {
            exitWithError("read() failed");
        }
        ostringstream ss;
        ss << "received Request";
        log(ss.str());
        sendResponse();
        close(client_socket);
    }

    int tcpServer::startServer()
    {
        m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socket == -1)
        {
            exitWithError("socket() failed");
            return 1;
        }

        if (::bind(m_socket, (struct sockaddr *)&m_socket_address, m_addr_length) < 0)
        {
            exitWithError("bind() failed");
            return 1;
        }
        return 0;
    }

    void tcpServer ::closeServer()
    {
        close(m_socket);
        close(m_new_socket);
        exit(0);
    }

    void tcpServer::startlistening()
    {
        if (listen(m_socket, 4) < 0)
        {
            exitWithError("listen() failed");
        }

        ostringstream ss;
        ss << "Listening on port " << m_port << std::endl;
        log(ss.str());

        int bytesReceived;
        while (true)
        {
            log("waiting");
            acceptConnection(m_new_socket);
            char buffer[BUFFER_SIZE] = {0};
            bytesReceived = read(m_new_socket, buffer, BUFFER_SIZE);
            if (bytesReceived < 0)
            {
                exitWithError("read() failed");
            }
            ostringstream ss;
            ss << "received Request";
            log(ss.str());

            sendResponse();
            // close(m_new_socket);
        }
    }

    void tcpServer::acceptConnection(int &new_socket)
    {
        new_socket = accept(m_socket, (struct sockaddr *)&m_socket_address, &m_addr_length);
        if (new_socket < 0)
        {
            exitWithError("accept() failed");
        }
    }

    std::string tcpServer::buildResponse()
    {
        std::ostringstream ss;
        ss << " HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 12\n\nHello world!";
        return ss.str();
    }

    void tcpServer ::sendResponse()
    {
        long sendBytes;
        sendBytes = write(m_new_socket, m_serverMsg.c_str(), m_serverMsg.size());
        if (sendBytes < 0)
        {
            exitWithError("write() failed");
        }
        std::ostringstream ss;
        ss << "Response sent";
        log(ss.str());
    }
}
