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
#include <vector>
#include <sys/wait.h>

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

    std::vector<pid_t> child_pids;  // Keep track of child process IDs

    while (i < this->num_workers) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            int clientfd;
            if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                std::cerr << "Socket creation error" << std::endl;
                exit(EXIT_FAILURE);  // Exit child process on error
            }

            struct sockaddr_in clientaddr;
            clientaddr.sin_family = AF_INET;
            clientaddr.sin_port = htons(PORT);

            int reuseaddr = 1;
            if (setsockopt(clientfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) {
                std::cerr << "setsockopt() => -1, errno=" << errno << std::endl;
                close(clientfd);
                exit(EXIT_FAILURE);  // Exit child process on error
            }

            // Convert IP address to binary form
            if (inet_pton(AF_INET, "127.0.0.1", &clientaddr.sin_addr) <= 0) {
                std::cerr << "Invalid address / Address not supported" << std::endl;
                close(clientfd);
                exit(EXIT_FAILURE);  // Exit child process on error
            }

            auto start_time = std::chrono::steady_clock::now();
            if (connect(clientfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
                std::cerr << "Connection failed, errno=" << errno << std::endl;
                close(clientfd);
                exit(EXIT_FAILURE);  // Exit child process on error
            }

            // Handle the connection in the child process
            ClientHandler handler(clientfd, start_time, waiting_time);
            handler.handleConnection(this->num_workers);

            // Close client socket and exit child process
            close(clientfd);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) {
            // Fork failed
            std::cerr << "Fork failed" << std::endl;
            return;
        } else {
            // Parent process: Store child PID
            child_pids.push_back(pid);
        }

        i++;
    }

    // Parent process: Wait for all child processes to complete
    for (pid_t pid : child_pids) {
        int status;
        pid_t result = waitpid(pid, &status, 0);  // Wait for specific child
        if (result == -1) {
            std::cerr << "Error waiting for child process, errno=" << errno << std::endl;
        } else if (WIFEXITED(status)) {
            std::cout << "Child process " << pid << " exited with status " << WEXITSTATUS(status) << std::endl;
        } else if (WIFSIGNALED(status)) {
            std::cerr << "Child process " << pid << " killed by signal " << WTERMSIG(status) << std::endl;
        }
    }

    waiting_time.flush();
    waiting_time.close();  // Close the file after the loop ends
}


