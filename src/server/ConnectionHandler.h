#pragma once

#include "TCPServer.h"

class ConnectionHandler {
    private:
        int fd;
        TCPServer& server; // Reference to the TCPServer instance

    public:
        explicit ConnectionHandler(int fd, TCPServer& server);
        std::string readMessage();
        void sendMessage(const std::string& msg);
        void handleConnection();
};