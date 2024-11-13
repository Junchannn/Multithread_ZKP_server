#include "TCPServer.h"
#include <iostream>
#include <csignal>

TCPServer* server_ptr = nullptr;

void signal_handler(int sig){
    if (server_ptr) {
        std::cout << "Server stopped" << std::endl; 
        server_ptr->stop();
    }
}

int main() 
{
    // create server - it starts automatically in constructor
    TCPServer server;
    server_ptr = &server;

    std::signal(SIGINT, signal_handler); 

    server.join();

    return 0;
}