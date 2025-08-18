#pragma once

#include <AppScene/AppInterface.h>
#include "../Screens/GraphScene/Graph.h"
#include "../Screens/MainMenuScreen/MainMenuScreen.h"

class App : public AppInterface
{
public:
	App(int threadCount);
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

