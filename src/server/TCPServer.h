#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <fstream>

class TCPServer {
public:
    static const int PORT = 1337;
    TCPServer();
    ~TCPServer();
    void start(); // Start server
    void stop(); // Stop server
    void join(); // Join server thread

    // std::atomic<int> connection_count{0};
    // std::mutex mtx;
    // std::condition_variable cv;

private:
    int efd; // Event file descriptor - used to tell server to stop
    int server_socket;
    std::thread m_thread; // Server thread
    void threadFunc();
};