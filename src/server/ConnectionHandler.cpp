#include "ConnectionHandler.h"
#include "../ZKP/DLP_ZKP.h"
#include <cassert>
#include <sys/socket.h>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <nlohmann/json.hpp>
#include <sys/syscall.h>

using json = nlohmann::json;

ConnectionHandler::ConnectionHandler(int fd, TCPServer& server) : fd(fd), server(server) {}


std::string ConnectionHandler::readMessage() {
    std::string msg(2048, '\0');    // buffer with 1024 length which is filled with NULL character
    
    int readBytes = recv(this->fd, msg.data(), msg.size(), 0);
    if (readBytes < 1) {
        std::cout << "Error in readMessage, readBytes: " << readBytes << std::endl;
        return "";
    }

    return msg;
}


void ConnectionHandler::sendMessage(const std::string& msg) {
    int n = send(this->fd, msg.c_str(), msg.size(), 0);
    if (n != static_cast<int>(msg.size())) {
        std::cout << "Error while sending message, message size: " << msg.size() << " bytes sent: " << std::endl;
    }
}
void ConnectionHandler::handleConnection() {
    pid_t tid = syscall(SYS_gettid);
    std::cout << "Connection handled at thread ID: " << tid << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    
    Verifier verifier = Verifier();
    verifier.gen_public_key();

    json pub_key;
    pub_key["g"] = serialize_ZZ(verifier.g);
    pub_key["p"] = serialize_ZZ(verifier.p);
    this->sendMessage(pub_key.dump());
    std::cout << "Sent public key: " << pub_key << std::endl;
    // receive a, y
    auto msg = this->readMessage();
    json recv_data = json::parse(msg);
    verifier.a = unserialize_ZZ(recv_data["a"]);
    verifier.y = unserialize_ZZ(recv_data["y"]);
    verifier.e = generate_random_number(verifier.g, verifier.order);
    json challenge;
    challenge["e"] = serialize_ZZ(verifier.e);

    //send e
    this->sendMessage(challenge.dump());
    //recv prove 
    ZZ z = unserialize_ZZ(json::parse(this->readMessage())["prove"]);
    //check prove
    if(verifier.verify(z))
        this->sendMessage("Authorized");
    else{
        this->sendMessage("Malicious");
    }
    {
        std::lock_guard<std::mutex> lock(this->server.mtx);
        // Critical section where shared resources are modified
        this->server.connection_count--;
    }
    this->server.cv.notify_all();
    shutdown(this->fd, SHUT_RDWR);
    close(this->fd);
    std::cout << "Terminated thread ID: "<< tid << std::endl;
    
}


