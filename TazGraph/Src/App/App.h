#pragma once

#include "GraphScreen/IMainGraph.h"
#include "../Graph.h"
#include "../MainMenuScreen/MainMenuScreen.h"

class App : public IMainGraph
{
public:
	App();
	~App();

    // Called on initialization
    virtual void onInit() override;
    // For adding all screens
    virtual void addScreens() override;
    // Called when exiting
    virtual void onExit() override;
private:

    std::unique_ptr<Graph> _graphplayScreen = nullptr;
    std::unique_ptr<MainMenuScreen> _mainMenuScreen = nullptr;

    //std::unique_ptr<EditorScreen> m_editorScreen = nullptr;
};

