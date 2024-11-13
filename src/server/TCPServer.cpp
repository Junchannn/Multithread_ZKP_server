#include "TCPServer.h"
#include "ConnectionHandler.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <sys/eventfd.h>
#include <cstring>
#include <poll.h>

const int LISTEN_QUEUE_SIZE = 50;
const int MAX_CONNECTIONS_AT_A_TIME = 11;

TCPServer::TCPServer() : efd(-1), server_socket(-1) {
    this->start();
}

TCPServer::~TCPServer() {
    this->stop();
}

void TCPServer::start() {
    
    // this->output_file.open("measure/waiting_time.txt");
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if (sockfd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return;
    }
       
    if (sockfd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return;
    }
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(sockfd);
        return;
    }

    if (listen(sockfd, LISTEN_QUEUE_SIZE) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(sockfd);
        return;
    }

    this->server_socket = sockfd;
    this->efd = eventfd(0, 0); // Create event file descriptor

    m_thread = std::thread(&TCPServer::threadFunc, this);
    pthread_setname_np(m_thread.native_handle(), "TCPServer");
}

void TCPServer::stop() {
    if (this->efd != -1) {
        uint64_t u = 1;
        write(this->efd, &u, sizeof(uint64_t)); // Notify the server to stop
        // this->output_file.close();
        this->efd = -1;
    }

    exit(0);
}

void TCPServer::join() {
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void TCPServer::threadFunc() {
    std::cout << "Server started " << this->PORT << std::endl;
    auto start_time = std::chrono::steady_clock::now();

    while (true) {
        auto now = std::chrono::steady_clock::now();
        //check if number of incoming requests is larger than serving ability of server
        if (connection_count < MAX_CONNECTIONS_AT_A_TIME) {
            int client_socket = accept(server_socket, nullptr, nullptr);
            //mutex to safely modify sharing resource
            if (client_socket >= 0) {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    connection_count++;
                }
                cv.notify_all();
                std::thread t([this, client_socket]() {
                    try {
                        ConnectionHandler handler(client_socket, *this);
                        handler.handleConnection();
                    } catch (const std::exception& e) {
                        std::cerr << "Exception in connection handler: " << e.what() << std::endl;
                    } catch (...) {
                        std::cerr << "Unknown exception in connection handler" << std::endl;
                    }
                });
                t.detach();
            } else {
                std::cerr << "Failed to accept connection: " << strerror(errno) << std::endl;
            }
        } else {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [this]{
                return this->connection_count < MAX_CONNECTIONS_AT_A_TIME;
            });
            auto wait_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - now).count();
            // if (wait_duration)
            //     this->output_file << wait_duration << std::endl;
        }

        // Check if we need to stop
        struct pollfd pfd;
        pfd.fd = this->efd;
        pfd.events = POLLIN;
        int ret = poll(&pfd, 1, 0); // Poll with a timeout of 0 to avoid blocking
        if (ret > 0 && (pfd.revents & POLLIN)) {
            uint64_t u;
            read(this->efd, &u, sizeof(uint64_t));
            std::cout << "Server stopping" << std::endl;
            break;
        }
    }
    
}