#include "HMIClient.h"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

HMIClient::HMIClient(const std::string& serverIp, int port)
    : serverIp_(serverIp), port_(port), sock_(-1) {}

HMIClient::~HMIClient() {
    stop();
}

void HMIClient::start() {
    running_ = true;
    clientThread_ = std::thread(&HMIClient::runClient, this);
}

void HMIClient::stop() {
    running_ = false;
    if (sock_ != -1) close(sock_);
    if (clientThread_.joinable()) clientThread_.join();
}

void HMIClient::runClient() {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, serverIp_.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return;
    }

    std::cout << "📡 Connecting to ADAS Gateway...\n";
    while (connect(sock_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "✅ Connected to ADAS Gateway!\n";

    char buffer[1024];
    while (running_) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(sock_, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) break;
        std::cout << "📊 Sensor Fusion Data: " << buffer << std::endl;
    }
}
