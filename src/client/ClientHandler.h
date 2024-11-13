#pragma once

#include "TCPClient.h"
#include <chrono>
#include <fstream>

class ClientHandler {
    private:
        int fd;
        int process_id;
        std::chrono::time_point<std::chrono::steady_clock> start_time;
        std::fstream& waiting_time;
    public:
        explicit ClientHandler(int fd, std::chrono::time_point<std::chrono::steady_clock> start_time, std::fstream& waiting_time);
        std::string readMessage();
        void sendMessage(const std::string& msg);
        void handleConnection(int num_workers);
};