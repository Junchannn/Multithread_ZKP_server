#include "ClientHandler.h"
#include "../ZKP/DLP_ZKP.h"
#include <cassert>
#include <sys/socket.h>
#include <errno.h>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

ClientHandler::ClientHandler(int fd, std::chrono::time_point<std::chrono::steady_clock> start_time, std::fstream& waiting_time) : 
        fd(fd), start_time(start_time), process_id(process_id), waiting_time(waiting_time){}



void ClientHandler::handleConnection(int num_workers) {
    std::string msg = this->readMessage();
    auto wait_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->start_time).count();
    this->waiting_time << num_workers << " " << wait_duration << std::endl;
    json pub_key = json::parse(msg);
    //recv pub_key
    ZZ g = unserialize_ZZ(pub_key["g"]);
    ZZ p = unserialize_ZZ(pub_key["p"]);
    ZZ w = generate_random_number(g, p - 1); // rand between g and order
    Prover prover = Prover(w, p ,g);

    //send data
    json send_data;
    send_data["a"] = serialize_ZZ(prover.gen_param());
    send_data["y"] = serialize_ZZ(prover.y);
    this->sendMessage(send_data.dump());
    ZZ e = unserialize_ZZ(json::parse(this->readMessage())["e"]);

    //gererate prove
    ZZ z = prover.gen_prove(e);
    json prove;
    prove["prove"] = serialize_ZZ(z);
    this->sendMessage(prove.dump());
    std::cout << this->readMessage() << std::endl; //Check for output
    fflush(stdout);
    shutdown(this->fd, SHUT_RDWR);
    close(this->fd);
    exit(0);
}

std::string ClientHandler::readMessage() {
    std::string msg(2048, '\0');    // buffer with 1024 length which is filled with NULL character      
    int readBytes = recv(this->fd, msg.data(), msg.size(), 0);
    if (readBytes < 1) {
        std::cout << "Error in readMessage, readBytes: " << readBytes << std::endl;
        return "";
    }

    return msg;
}


void ClientHandler::sendMessage(const std::string& msg) {
    int n = send(this->fd, msg.c_str(), msg.size(), 0);
    if (n != static_cast<int>(msg.size())) {
        std::cout << "Error while sending message, message size: " << msg.size() << " bytes sent: " << std::endl;
    }
}


