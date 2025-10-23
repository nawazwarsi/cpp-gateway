#pragma once
#include <string>
#include <thread>
#include <atomic>

class HMIClient {
public:
    HMIClient(const std::string& serverIp, int port);
    ~HMIClient();

    void start();
    void stop();

private:
    void runClient();

    std::string serverIp_;
    int port_;
    int sock_;
    std::thread clientThread_;
    std::atomic<bool> running_{false};
};
