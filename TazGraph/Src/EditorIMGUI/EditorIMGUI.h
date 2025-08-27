#pragma once

#include "../Map/Map.h"
#include <algorithm>
#include <vector>
#include <string>
#include "ImGuiInterface/ImGuiInterface.h"
#include "imguiComboAutoselect/imgui_combo_autoselect.h"
#include "BaseFPSLimiter/BaseFPSLimiter.h"

#include "Components/CustomFunctions/CustomFunctions.h"
#include "Components/Minimap/Minimap.h"
#include "Components/OrientationBox/OrientationBox.h"

#include "./Components/SliderRotateZ.h"

#include <Renderers/FrameBuffer/Framebuffer.h>

namespace fs = std::filesystem;

// it is to provide the ImgUI functions for the whole project
class EditorIMGUI : public ImGuiInterface {
private:
	int _currentLinksPathIndex = 0;
	
	Minimap _minimap;
	OrientationBox _orientationBox;

	// Note: Switch this to true to enable dockspace
	bool _dockingEnabled = true;
	int _lastEntityDisplayed = 0;
public:
	SliderRotateZ sliderRotate;
	CustomFunctions _customFunctions;

	int newNodesCount = 0;
	int newLinksCount = 0;




	bool isMouseInSecondColumn = false;

	EditorIMGUI();

	~EditorIMGUI();

	bool* getDockspaceRef();
	void MenuBar();

	bool isMouseOnWidget(const std::string& widgetName);
	void SavingUI(Map* map);
	void NewMapUI();
	char* LoadingUI();
	void SceneViewport(
		const BaseFPSLimiter& baseFPSLimiter,
		Manager& manager,
		Framebuffer& textureId, 
		Framebuffer& m_minimap_fb,
		ImVec2& storedWindowPos, ImVec2& storedWindowSize);
	void updateIsMouseInSecondColumn();
	void showHoveredEntity(Manager& manager, glm::vec2 mousePos, Entity* onHoverEntity);
	void ShowEntityComponents(glm::vec2 mousePos, Entity* displayedEntity, Manager& manager);
	void ShowSceneControl(glm::vec2 mousePos, Manager& manager);
	void StartPollingComponent(Entity* entity, const std::string& fileName);
};