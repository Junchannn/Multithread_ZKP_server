#include "TCPClient.h"
#include "ClientHandler.h"
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <fstream>

#define TIMEOUT 1000
#define PORT 1337  // Define PORT if it's not defined elsewhere

TCPClient::TCPClient(int num_workers)
    : num_workers(num_workers), efd(-1){  // Initialize efd to -1
    this->start();
}

TCPClient::~TCPClient() {
    this->stop();
    if (this->efd != -1){
        shutdown(this->efd, SHUT_RDWR);
        close(this->efd);
    }
}

void TCPClient::stop() {
    this->join();
    std::cout << "TCP client stopped" << std::endl;
}


void TCPClient::join() {
    if (this->m_thread.joinable()) {
        this->m_thread.join();
    }
}

void TCPClient::start() {
    if (this->m_thread.joinable()) {
        std::cerr << "Thread is already running" << std::endl;
        return;
    }
    if (this->efd != -1) {
        close(this->efd);
    }

    this->efd = eventfd(0, 0);
    if (this->efd == -1) {
        std::cerr << "eventfd() failed, errno=" << errno << std::endl;
        return;
    }
    // Create thread
    // this->m_thread = std::thread([this]() { this->threadFunc(); });

    // // Set name for thread
    // pthread_setname_np(this->m_thread.native_handle(), "TCPClient");
    this->processFunc();
}
void TCPClient::processFunc() {
    int i = 0;
    std::fstream waiting_time;
    // Open file only once before the loop starts
    waiting_time.open("measure/waiting_time.txt", std::ios::out | std::ios::app);  // Use append mode to avoid overwriting
    if (!waiting_time.is_open()) {
        std::cerr << "Failed to open file waiting_time.txt" << std::endl;
        return; // Handle file open failure
    }
    waiting_time << "Number of workers: " << this->num_workers << std::endl;
    while (i < this->num_workers) {
        pid_t pid = fork();
        if (pid == 0) {
            int clientfd;
            if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                std::cerr << "Socket creation error" << std::endl;
                return;
            }
            struct sockaddr_in clientaddr;
            clientaddr.sin_family = AF_INET;
            clientaddr.sin_port = htons(PORT);
            int reuseaddr = 1;
            if (setsockopt(clientfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) {
                std::cout << "setsockopt() => -1, errno=" << errno << std::endl;
            }
            // Convert IP address to binary form
            if (inet_pton(AF_INET, "127.0.0.1", &clientaddr.sin_addr) <= 0) {
                std::cerr << "Invalid address / Address not supported" << std::endl;
                close(clientfd);
                return;
            }

            const int max_retries = 5;
            int retry_count = 0;
            auto start_time = std::chrono::steady_clock::now();
            pid_t check = connect(clientfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
        // std::cout << clientfd << std::endl;
        // std::cout << "Connected to server" << std::endl;
        // std::thread t([this, clientfd]() {
        //     try {
        //         ClientHandler handler(clientfd, *this);
        //         handler.handleConnection();
        //     } catch (const std::exception& e) {
        //         std::cerr << "Exception in connection handler: " << e.what() << std::endl;
        //     } catch (...) {
        //         std::cerr << "Unknown exception in connection handler" << std::endl;
        //     }
        // });
        // pthread_setname_np(t.native_handle(), "Client child");
        // t.detach();
            // std::cout << "current pid: " << getpid() << std::endl;
            ClientHandler handler(clientfd, start_time, waiting_time);
            handler.handleConnection();
            return;
        } else if (pid < 0) {
            std::cerr << "Fork failed" << std::endl;
            return;
        }
        i++;
    }
    waiting_time.flush();
    waiting_time.close();  // Close the file after the loop ends

    // Loop through and close each file descriptor on exit
}


