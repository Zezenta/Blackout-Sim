#include <iostream>
#include <string>
#include <chrono>
#include <conio.h>
#include <thread>
#include <atomic>

class Generator {
public:
    Generator(std::string newname, double lt, double v) : name(newname), volts(v), liters(lt) {}

    std::string name;
    std::atomic<bool> state{false};
    double volts;
    double liters;
    std::chrono::steady_clock::time_point startConsumption;
    std::chrono::steady_clock::time_point startTimer;

    void showparams() {
        std::cout << "The parameters of the " << name << " generator are:" << std::endl;
        std::cout << "State: " << ((state == true) ? "On" : "Off") << std::endl;
        std::cout << "Liters left: " << liters << std::endl;
        std::cout << "Voltage configuration: " << volts << std::endl;
    }

    void enable() {
        if (!state) {
            state = true;
            startConsumption = std::chrono::steady_clock::now();
            (liters > 0) ? (std::cout << "Generator " << name << " is now running." << std::endl) : (std::cout << "Generator " << name  << " has not enough fuel to work." << std::endl);
            startTimer = std::chrono::steady_clock::now();

            std::thread([&] {
                while (state) {
                    updateLiters();
                    if (liters <= 0) {
                        disable();
                        liters=0;
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }).detach();
        } else {
            std::cout << "Generator " << name << " is already running." << std::endl;
        }
    }

    void disable() {
        if (state) {
            state = false;
            auto endTimer = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTimer - startTimer);
            std::cout << "Generator " << name << " was running for " << duration.count() << " seconds." << std::endl;
        } else {
            std::cout << "Generator " << name << " is already disabled." << std::endl;
        }
    }

    void updateLiters() {
        auto endConsumption = std::chrono::steady_clock::now();
        auto consumage = std::chrono::duration_cast<std::chrono::seconds>(endConsumption - startConsumption);
        int time = consumage.count();
        liters -= (state) ? (time * volts) / 10 : 0;
        startConsumption = std::chrono::steady_clock::now();
    }
};

int main() {
    Generator generator1("Principal", 10, 10);
    generator1.showparams();

    while (true) {
        if (_kbhit()) {
            int key = _getch();
            switch (key) {
                case 'X':
                case 'x':
                    return 0;
                case 'E':
                case 'e':
                    generator1.enable();
                    break;
                case 'D':
                case 'd':
                    generator1.disable();
                    break;
                case 'I':
                case 'i':
                    generator1.showparams();
                    break;
                default:
                    // Do nothing for other keys
                    break;
            }
        }
    }
}