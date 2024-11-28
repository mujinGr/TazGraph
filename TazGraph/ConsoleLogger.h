#pragma once

#include <iostream>
#include <string>
#include <ctime>

namespace nmConsoleLogger {
    void printCurrentTime() {
        std::time_t now = std::time(nullptr);
        struct tm timeInfo;
        localtime_s(&timeInfo, &now);

        char timestamp[20]; // Sufficient space for the timestamp
        strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", &timeInfo);

        std::cout << timestamp;
    }
    
    extern void log(const std::string& message) {
        printCurrentTime();
        std::cout << " " << message << std::endl;
    }

    extern void error(const std::string& errorMessage) {
        printCurrentTime();
        std::cerr << " [ERROR] " << errorMessage << std::endl;
    }
};
