#include "TCPClient.h"
#include <iostream>
#include <csignal> 

void signal_handler(int sig){
    std::cout << "Client farm Stop" << std::endl;
    exit(sig);
}
int main(int argc, char** argv) 
{
    if (argc != 2){
        std::cout << "Usage ./prover_client <num_client>" << std::endl;
        return 0;
    }
    int num_client = std::stoi(argv[1]);
    std::signal(SIGINT, signal_handler); 
    // create server - it starts automatically in constructor
    TCPClient client = TCPClient(num_client);

    // server.stop(); // we could stop the server this way

    // wait for server thread to end
    client.join();

    return 0;
}