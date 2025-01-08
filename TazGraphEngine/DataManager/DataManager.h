#pragma once

#include <map>
#include <memory>
#include <string>

class DataManager {
public:
    // Gets the single instance of CameraManager (singleton)
    static DataManager& getInstance() {
        static DataManager instance; // Guaranteed to be destroyed. Instantiated on first use.
        return instance;
    }

    DataManager() {}

    std::string mapToLoad;
};