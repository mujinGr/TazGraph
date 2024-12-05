#pragma once

#include <map>
#include <memory>
#include <string>
#include "ICamera.h"
#include "PerspectiveCamera.h"
#include "OrthoCamera.h"

class CameraManager {
public:
    // Gets the single instance of CameraManager
    static CameraManager& getInstance() {
        static CameraManager instance; // Guaranteed to be destroyed. Instantiated on first use.
        return instance;
    }

    CameraManager() {}

    void addCamera(const std::string& name, std::shared_ptr<ICamera> camera) {
        cameras[name] = camera;
    }

    std::shared_ptr<ICamera> getCamera(const std::string& name) {
        auto it = cameras.find(name);
        if (it != cameras.end()) {
            return it->second;
        }
        return nullptr; // or a default camera2D.worldLocation if you prefer
    }

    void initializeCameras() {
        auto mainCamera = std::make_shared<PerspectiveCamera>();
        // Configure your mainCamera as needed
        CameraManager::getInstance().addCamera("main", mainCamera);

        auto hudCamera = std::make_shared<OrthoCamera>();
        // Configure your hudCamera as needed, usually orthographic with fixed positioning
        CameraManager::getInstance().addCamera("hud", hudCamera);

        auto mainMenu_mainCamera = std::make_shared<PerspectiveCamera>(glm::vec3(400.f, 512.f, -780.0f), glm::vec3(400.f, 512.f, 0.f));
        // Configure your mainCamera as needed
        CameraManager::getInstance().addCamera("mainMenu_main", mainMenu_mainCamera);

        auto mainMenu_hudCamera = std::make_shared<OrthoCamera>();
        // Configure your hudCamera as needed, usually orthographic with fixed positioning
        CameraManager::getInstance().addCamera("mainMenu_hud", mainMenu_hudCamera);
    }

private:
    std::map<std::string, std::shared_ptr<ICamera>> cameras;
};