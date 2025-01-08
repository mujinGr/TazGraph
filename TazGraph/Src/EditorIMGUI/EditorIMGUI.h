#pragma once

#include "../Map/Map.h"
#include <algorithm>
#include <vector>
#include <string>
#include <filesystem>
#include "ImGuiInterface/ImGuiInterface.h"
#include "BaseFPSLimiter/BaseFPSLimiter.h"
#include "GOS/Components.h"

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
public:
	EditorIMGUI();

	~EditorIMGUI();

	bool isSaving();
	bool isLoading();
	void setLoading(bool loading);
	bool isGoingBack();
	void SetGoingBack(bool goingBack);

	void BackGroundUIElement(bool& renderDebug, glm::vec2 mouseCoords, const Manager& manager, Entity* selectedEntity, float(&backgroundColor)[4], int cell_size);
	void FileActions();
	void FPSCounter(const BaseFPSLimiter& baseFPSLimiter);
	void ReloadAccessibleFiles();
	void SavingUI(Map* map);
	char* LoadingUI();
	void MainMenuUI(std::function<void()> onStartSimulator, std::function<void()> onLoadSimulator, std::function<void()> onExitSimulator);
};