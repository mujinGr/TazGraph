#pragma once

#include "../Map/Map.h"
#include <algorithm>
#include <vector>
#include <string>
#include "ImGuiInterface/ImGuiInterface.h"
#include "imguiComboAutoselect/imgui_combo_autoselect.h"
#include "BaseFPSLimiter/BaseFPSLimiter.h"

#include <Renderers/FrameBuffer/Framebuffer.h>

namespace fs = std::filesystem;

// it is to provide the ImgUI functions for the whole project
class EditorIMGUI : public ImGuiInterface {
private:
	// Note: Switch this to true to enable dockspace
	bool _dockingEnabled = true;
public:
	EditorIMGUI();

	~EditorIMGUI();

	bool* getDockspaceRef();

	bool isMouseOnWidget(const std::string& widgetName);
};