#pragma once
#include <unordered_map>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <fstream>

class Gateway {
public:
    explicit Gateway(int port);
    ~Gateway();

    void start();
    void stop();

    void onSensorData(const std::string& name, double value);

private:
    void runServer();
    void logToCSV();

    int port_;
    std::unordered_map<std::string, double> sensorData_;
    std::mutex dataMutex_;
    std::thread serverThread_;
    std::atomic<bool> running_{false};
    std::ofstream logFile_;
};
