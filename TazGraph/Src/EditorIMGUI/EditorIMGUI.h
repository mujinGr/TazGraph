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

class EditorIMGUI : public ImGuiInterface {
private:
	bool _isSaving = false;
public:
	EditorIMGUI();

	~EditorIMGUI();

	bool isSaving();

	void BackGroundUIElement(bool& renderDebug, glm::vec2 mouseCoords, const Manager& manager, Entity* selectedEntity, float(&backgroundColor)[4], int cell_size);
	void FileActions(Map* map);
	void FPSCounter(const BaseFPSLimiter& baseFPSLimiter);
	void InputFileName();
};