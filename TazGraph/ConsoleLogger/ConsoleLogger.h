#pragma once

#include <iostream>
#include <string>
#include <ctime>

namespace nmConsoleLogger {
    void printCurrentTime();
    
    void log(const std::string& message);

    void error(const std::string& errorMessage);
};
