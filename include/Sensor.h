#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#include <random>
#include <iostream>

class Sensor {
public:
    using Callback = std::function<void(const std::string&, double)>;

    Sensor(const std::string& name, Callback cb);
    ~Sensor();

    void start();
    void stop();

private:
    void run();
    std::string name_;
    Callback callback_;
    std::thread thread_;
    std::atomic<bool> running_{false};
};
