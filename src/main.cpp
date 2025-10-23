#include "Gateway.h"
#include "Sensor.h"
#include "HMIClient.h"
#include <thread>

int main() {
    Gateway gateway(8080);
    auto callback = [&](const std::string& name, double value) {
        gateway.onSensorData(name, value);
    };

    Sensor radar("Radar", callback);
    Sensor camera("Camera", callback);
    Sensor lidar("Lidar", callback);

    // Start gateway and sensors
    gateway.start();
    radar.start();
    camera.start();
    lidar.start();

    // Start HMI client after a short delay (so server starts first)
    std::this_thread::sleep_for(std::chrono::seconds(2));
    HMIClient client("127.0.0.1", 8080);
    client.start();

    std::this_thread::sleep_for(std::chrono::seconds(15));

    // Stop all
    radar.stop();
    camera.stop();
    lidar.stop();
    gateway.stop();
    client.stop();

    return 0;
}
