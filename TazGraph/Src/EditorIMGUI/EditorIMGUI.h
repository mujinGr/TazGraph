#pragma once

#include "../Map/Map.h"
#include <algorithm>
#include <vector>
#include <string>
#include <filesystem>
#include "ImGuiInterface/ImGuiInterface.h"
#include "BaseFPSLimiter/BaseFPSLimiter.h"
#include "GECS/Components.h"
#include "GECS/UtilComponents.h"

namespace fs = std::filesystem;

// it is to provide the ImgUI functions for the whole project
class EditorIMGUI : public ImGuiInterface {
private:
	std::vector<std::string> _fileNames;
	ImGui::ComboAutoSelectData _data;
	bool _filesLoaded = false;

	bool _isSaving = false;
	bool _isLoading = false;
	bool _goingBack = false;

	// Note: Switch this to true to enable dockspace
	bool _dockingEnabled = true;

public:
	EditorIMGUI();

	~EditorIMGUI();

	bool isSaving();
	bool isLoading();
	void setLoading(bool loading);
	bool isGoingBack();
	void SetGoingBack(bool goingBack);

	bool* getDockspaceRef();
	void MenuBar();

	void BackGroundUIElement(bool& renderDebug, glm::vec2 mouseCoords, Manager& manager, Entity* selectedEntity, float(&backgroundColor)[4], int cell_size);
	void FPSCounter(const BaseFPSLimiter& baseFPSLimiter);
	void ReloadAccessibleFiles();
	void SavingUI(Map* map);
	char* LoadingUI();
	void MainMenuUI(std::function<void()> onStartSimulator, std::function<void()> onLoadSimulator, std::function<void()> onExitSimulator);
	void ShowAllEntities(Manager& manager, float& m_nodeRadius);
	void SceneViewport(uint32_t textureId, ImVec2& storedWindowPos, ImVec2& storedWindowSize);
	void SceneTabs();
};