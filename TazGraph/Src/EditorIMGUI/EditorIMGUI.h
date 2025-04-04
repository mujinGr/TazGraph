#pragma once

#include "../Map/Map.h"
#include <algorithm>
#include <vector>
#include <string>
#include <filesystem>
#include "ImGuiInterface/ImGuiInterface.h"
#include "BaseFPSLimiter/BaseFPSLimiter.h"

#include "CustomFunctions/CustomFunctions.h"

namespace fs = std::filesystem;

// it is to provide the ImgUI functions for the whole project
class EditorIMGUI : public ImGuiInterface {
private:
	std::vector<std::string> _fileNames;
	std::vector<std::string> _pollingFileNames;
	ImGui::ComboAutoSelectData _data;
	bool _filesLoaded = false;

	bool _isSaving = false;
	bool _isLoading = false;
	bool _goingBack = false;

	CustomFunctions _customFunctions;

	// Note: Switch this to true to enable dockspace
	bool _dockingEnabled = true;
public:
	bool isMouseInSecondColumn = false;


	
	int last_activeLayout = 0;
	int activeLayout = 0;

	EditorIMGUI();

	~EditorIMGUI();

	bool isSaving();
	bool isLoading();
	void setLoading(bool loading);
	bool isGoingBack();
	void SetGoingBack(bool goingBack);

	void updateFileNamesInAssets();

	void updatePollingFileNamesInAssets();

	bool* getDockspaceRef();
	void MenuBar();

	bool isMouseOnWidget(const std::string& widgetName);
	void BackGroundUIElement(bool& renderDebug, glm::vec2 mouseCoords, glm::vec2 mouseCoords2, Manager& manager, Entity* selectedEntity, float(&backgroundColor)[4], int cell_size);
	void FPSCounter(const BaseFPSLimiter& baseFPSLimiter);
	void ReloadAccessibleFiles();
	void SavingUI(Map* map);
	char* LoadingUI();
	void MainMenuUI(std::function<void()> onStartSimulator, std::function<void()> onLoadSimulator, std::function<void()> onExitSimulator);
	void ShowAllEntities(Manager& manager, float& m_nodeRadius);
	void availableFunctions();
	void SceneViewport(uint32_t textureId, ImVec2& storedWindowPos, ImVec2& storedWindowSize);
	void scriptResultsVisualization(Manager& manager, std::vector<std::pair<Entity*, glm::vec3>>& m_selectedEntities);
	std::string SceneTabs();
	void ShowFunctionExecutionResults();
	void updateIsMouseInSecondColumn();
	void ShowStatisticsAbout(glm::vec2 mousePos, Entity* displayedEntity, Manager& manager);
	void ShowSceneControl(glm::vec2 mousePos, Manager& manager);
	void StartPollingComponent(Entity* entity, const std::string& fileName);
};