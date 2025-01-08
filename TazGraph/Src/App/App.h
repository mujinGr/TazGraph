#pragma once

#include "GraphScreen/AppInterface.h"
#include "../Graph.h"
#include "../MainMenuScreen/MainMenuScreen.h"

class App : public AppInterface
{
public:
	App();
	~App();

    // Called on initialization
    virtual void onInit() override;
    // For adding all screens
    virtual void addScenes() override;
    // Called when exiting
    virtual void onExit() override;
private:

    std::unique_ptr<Graph> _graphplayScreen = nullptr;
    std::unique_ptr<MainMenuScreen> _mainMenuScreen = nullptr;

    //std::unique_ptr<EditorScreen> m_editorScreen = nullptr;
};

