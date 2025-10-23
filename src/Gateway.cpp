#include "Gateway.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

Gateway::Gateway(int port) : port_(port) {
    logFile_.open("sensor_fusion_log.csv", std::ios::out);
    logFile_ << "Timestamp,Sensor,Value\n";
}

Gateway::~Gateway() {
    stop();
    if (logFile_.is_open())
        logFile_.close();
}

void Gateway::start() {
    running_ = true;
    serverThread_ = std::thread(&Gateway::runServer, this);
}

void Gateway::stop() {
    running_ = false;
    if (serverThread_.joinable())
        serverThread_.join();
}

void Gateway::onSensorData(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    sensorData_[name] = value;
    logToCSV(); // log new data
}

void Gateway::logToCSV() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    for (auto& [sensor, val] : sensorData_) {
        logFile_ << std::ctime(&now) << "," << sensor << "," << val << "\n";
    }
}

void Gateway::runServer() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);

    std::cout << "🚀 Gateway server listening on port " << port_ << "...\n";

    int client_fd = accept(server_fd, nullptr, nullptr);
    std::cout << "✅ HMI client connected.\n";

    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        std::ostringstream oss;
        oss << "{";
        {
            std::lock_guard<std::mutex> lock(dataMutex_);
            for (auto it = sensorData_.begin(); it != sensorData_.end(); ++it) {
                oss << "\"" << it->first << "\":" << it->second;
                if (std::next(it) != sensorData_.end())
                    oss << ", ";
            }
        }
        oss << "}";

        std::string jsonData = oss.str();
        send(client_fd, jsonData.c_str(), jsonData.size(), 0);
    }

    close(client_fd);
    close(server_fd);
}
