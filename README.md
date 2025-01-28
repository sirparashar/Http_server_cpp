

# HTTP TCP Server

This project implements a simple HTTP TCP server using C++ and Boost.Beast library. The server listens for incoming HTTP requests and responds with a "Hello world!" message.

## Prerequisites

- C++17 compatible compiler (e.g., `g++`)  
- Boost library (including Boost.Asio and Boost.Beast)  
- Make  

## Building the Project

To build the project, run the following command in the root directory:

```
make
./server

g++ -std=c++17 client/client.cpp -o client
./client
```
