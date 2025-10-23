#include "Sensor.h"

Sensor::Sensor(const std::string& name, Callback cb)
    : name_(name), callback_(cb) {}

Sensor::~Sensor() {
    stop();
}

void Sensor::start() {
    running_ = true;
    thread_ = std::thread(&Sensor::run, this);
}

void Sensor::stop() {
    running_ = false;
    if (thread_.joinable()) thread_.join();
}

void Sensor::run() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 100.0);

    while (running_) {
        double reading = dist(gen);
        callback_(name_, reading);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
