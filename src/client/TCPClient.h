#pragma once

#include <thread>
#include <chrono>
#include <fstream>

class TCPClient{
    public:
        void start(); //start server
        void stop(); //stop server
        void join(); //join new thread
        TCPClient(int num_workers);
        ~TCPClient();

    private:
        int efd;
        int num_workers;
        std::thread m_thread;
        void processFunc();
};