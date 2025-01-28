#include "http_tcpServer.h"
#include <iostream>
#include <sys/socket.h>
#include <sstream>
#include <unistd.h>
#include <thread>

std::mutex gQueue_lock;
std::condition_variable q_cond_var;
std::queue<int> m_client_queue;

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
        // close(m_new_socket);
        exit(0);
    }

    void tcpServer::startlistening()
    {
        if (listen(m_socket, 50) < 0)
        {
            exitWithError("listen() failed");
        }

        ostringstream ss;
        ss << "Listening on port " << m_port << std::endl;
        log(ss.str());

        const int threads_count = 6;
        std::vector<std::thread> threads;
        for (int i = 0; i < threads_count; i++)
        {
            threads.push_back(std::thread(&tcpServer::workerThread, this, i));
            std::cout << "working thread" << i << std::endl;
        }
        int bytesReceived;
        while (true)
        {
            // log("waiting");
            int new_socket = acceptConnection(m_new_socket);
            // char buffer[BUFFER_SIZE] = {0};
            // bytesReceived = read(m_new_socket, buffer, BUFFER_SIZE);
            // if (bytesReceived < 0)
            // {
            //     exitWithError("read() failed");
            // }
            // ostringstream ss;
            // ss << "received Request";
            // log(ss.str());

            // sendResponse();
            // // close(m_new_socket);
            {
                std::unique_lock<std::mutex> lock(gQueue_lock);
                m_client_queue.push(new_socket);
            }

            q_cond_var.notify_one();
        }
        for (auto &thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }

    int tcpServer::acceptConnection(int &new_socket)
    {
        new_socket = accept(m_socket, (struct sockaddr *)&m_socket_address, &m_addr_length);
        if (new_socket < 0)
        {
            exitWithError("accept() failed");
        }
        else
        {
            return new_socket;
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
            std::ostringstream ss;
            ss << "write() failed with error: " << strerror(errno);
            log(ss.str());
            exitWithError("write() failed");
        }
        std::ostringstream ss;
        ss << "Response sent";
        log(ss.str());
    }

    void tcpServer::workerThread(int thread_id)
    {
        while (true)
        {
            int clientSocket;
            {
                std::unique_lock<std::mutex> lock(gQueue_lock);
                q_cond_var.wait(lock, []
                                { return !m_client_queue.empty(); });
                std::cout << "thread " << thread_id << "woke up" << std::endl;
                clientSocket = m_client_queue.front();
                m_client_queue.pop();
            }
            handleClient(clientSocket);
        }
    }
}
