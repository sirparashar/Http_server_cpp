#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>

#define SERVER_PORT 8080
#define SERVER_ADDRESS "127.0.0.1"
#define BUFFER_SIZE 1024

void sendRequest()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection Failed" << std::endl;
        return;
    }

    std::string message = "Hello from client";
    send(sock, message.c_str(), message.size(), 0);
    // std::cout << "Message sent" << std::endl;

    int valread = read(sock, buffer, BUFFER_SIZE);
    std::cout << "Server response: " << std::endl;

    close(sock);
}

int main()
{
    const int numRequests = 1000; // Number of simultaneous requests
    std::vector<std::thread> threads;

    for (int i = 0; i < numRequests; ++i)
    {
        threads.emplace_back(sendRequest);
    }

    for (auto &thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    return 0;
}