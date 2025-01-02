#pragma once

#include "../Map/Map.h"
#include <algorithm>
#include "ImGuiInterface/ImGuiInterface.h"
#include "BaseFPSLimiter/BaseFPSLimiter.h"

class EditorIMGUI : public ImGuiInterface {
public:
	EditorIMGUI();

	~EditorIMGUI();

	void BackGroundUIElement();
	void FileActions(Map* map);
	void FPSCounter(const BaseFPSLimiter& baseFPSLimiter);
};