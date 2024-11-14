#pragma once

#include "TCPServer.h"

class ConnectionHandler {
    private:
        int fd;

    public:
        explicit ConnectionHandler(int fd);
        std::string readMessage();
        void sendMessage(const std::string& msg);
        void handleConnection();
};