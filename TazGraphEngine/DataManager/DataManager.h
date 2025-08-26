#pragma once

#include <map>
#include <memory>
#include <string>
#include <filesystem>

#include <imguiComboAutoselect/imgui_combo_autoselect.h>

namespace fs = std::filesystem;

class DataManager {
public:
    // Gets the single instance of CameraManager (singleton)
    static DataManager& getInstance() {
        static DataManager instance; // Guaranteed to be destroyed. Instantiated on first use.
        return instance;
    }

    DataManager() {}

    ImGui::ComboAutoSelectData data;
    ImGui::ComboAutoSelectData pathData;

    std::string mapToLoad;

    std::vector<std::string> fileNames;
    std::vector<std::string> pollingFileNames;
    std::vector<std::string> pathsFileNames;
    std::string pathLoading;

    bool filesLoaded = false;

    bool saving = false;
    bool startingNew = false;
    bool loading = false;
    bool loadingPath = false;
    bool goingBack = false;

    void setPathLoading(bool loading)
    {
        loadingPath = loading;
    }

    bool isSaving() {
        return saving;
    }

    void setNewMap(bool m_startingNew)
    {
        startingNew = m_startingNew;
    }

    void setLoading(bool m_loading)
    {
        loading = m_loading;
    }

    bool isStartingNew()
    {
        return startingNew;
    }

    bool isLoading()
    {
        return loading;
    }

    bool isLoadingPath()
    {
        return loadingPath;
    }

    bool isGoingBack()
    {
        return goingBack;
    }

    std::string getPathLoading() {
        return pathLoading;
    }

    void SetGoingBack(bool m_goingBack) {
        goingBack = m_goingBack;
    }

    void updateFileNamesInAssets() {
        fileNames.clear();
        const std::string path = "assets/Maps"; // Directory path
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                fileNames.push_back(entry.path().filename().string()); // Add file name to vector
            }
        }
    }

    void updatePollingFileNamesInAssets() {
        pollingFileNames.clear();
        const std::string path = "assets/Maps/Polling"; // Directory path
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                pollingFileNames.push_back(entry.path().filename().string()); // Add file name to vector
            }
        }
    }

    void updatePathFileNamesInAssets() {
        pathsFileNames.clear();
        const std::string path = "assets/Paths"; // Directory path
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                pathsFileNames.push_back(entry.path().filename().string()); // Add file name to vector
            }
        }
        pathsFileNames.push_back(">Reset");
    }

    void ReloadAccessibleFiles() {
        if (!filesLoaded) {
            updateFileNamesInAssets();
            updatePollingFileNamesInAssets();
            updatePathFileNamesInAssets();

            filesLoaded = true; // Set to true so we don't reload unnecessarily
        }
    }
};