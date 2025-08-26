#include "EditorIMGUI.h"
#include "GECS/Core/GECSUtil.h"

EditorIMGUI::EditorIMGUI() {
	DataManager::getInstance().ReloadAccessibleFiles();
}

EditorIMGUI::~EditorIMGUI() {

}




bool* EditorIMGUI::getDockspaceRef()
{
	return &_dockingEnabled;
}

void EditorIMGUI::MenuBar() {
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save")) {
				DataManager::getInstance().saving = true;
			}
			if (ImGui::MenuItem("Load")) {
				DataManager::getInstance().loading = true;
			}
			if (ImGui::MenuItem("Back")) {
				DataManager::getInstance().goingBack = true;
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

}

bool EditorIMGUI::isMouseOnWidget(const std::string& widgetName)
{
	ImGuiContext& g = *ImGui::GetCurrentContext(); // Get ImGui context

	for (ImGuiWindow* window : g.Windows) {
		if (window->Name == widgetName && !(window->Hidden || (window->Active == false))) {
			ImVec2 widgetPos = window->Pos;
			ImVec2 widgetSize = window->Size;

			ImVec2 mousePos = ImGui::GetMousePos();
			
			return (mousePos.x >= widgetPos.x && mousePos.x <= (widgetPos.x + widgetSize.x) &&
				mousePos.y >= widgetPos.y && mousePos.y <= (widgetPos.y + widgetSize.y));
		}
	}

	return false;
}

void EditorIMGUI::RightColumnUIElement(Manager& manager, float* nodeRadius) {
	availableFunctions();
	ShowFunctionExecutionResults();
	ImGui::Separator();
	ShowAllEntities(manager, *nodeRadius);
}

void EditorIMGUI::FPSCounter(const BaseFPSLimiter& baseFPSLimiter) {

	ImGui::Begin("Performance");
	ImGui::Text("FPS: %f", baseFPSLimiter.fps);
	if (ImPlot::BeginPlot("FPS Plot")) {
		int plot_count = std::min(baseFPSLimiter.fps_history_count,
			baseFPSLimiter.fpsHistoryIndx); // Ensuring we do not read out of bounds
		int plot_offset = std::max(0,
			baseFPSLimiter.fpsHistoryIndx - baseFPSLimiter.fps_history_count); // Ensure a positive offset

		ImPlot::SetupAxesLimits(0, 100, 0, 200);

		ImPlot::PlotLine("FPS", &baseFPSLimiter.fpsHistory[0], plot_count);

		ImPlot::EndPlot();
	}
	ImGui::End();

}



void EditorIMGUI::SavingUI(Map* map) {

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize(400, 100); // Desired window size
	ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f,
		(io.DisplaySize.y - windowSize.y) * 0.5f);

	ImGui::SetNextWindowPos(windowPos);
	ImGui::SetNextWindowSize(windowSize);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	bool open = true;
	ImGui::Begin("Saving...", &open, window_flags);

	DataManager::getInstance().ReloadAccessibleFiles();

	DataManager::getInstance().data.SetSelectData(std::move(DataManager::getInstance().fileNames));

	if (ImGui::ComboAutoSelect("Select File", DataManager::getInstance().data)) {
	}
	ImGui::NewLine();
	float windowWidth = ImGui::GetContentRegionAvail().x;
	float buttonWidth = 100; // Define the button width you want
	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center the button

	if (ImGui::Button("Save", ImVec2(buttonWidth, 0))) {
		map->saveMapAsText(DataManager::getInstance().data.input); // save Map that is selected
		DataManager::getInstance().saving = false;
		DataManager::getInstance().filesLoaded = false;
	}

	if (!open) {
		DataManager::getInstance().saving = false;
	}

	ImGui::End();
}


void EditorIMGUI::NewMapUI() {
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize(400, 100); // Desired window size
	ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f,
		(io.DisplaySize.y - windowSize.y) * 0.5f);

	ImGui::SetNextWindowPos(windowPos);
	ImGui::SetNextWindowSize(windowSize);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	bool open = true;
	ImGui::Begin("New Map...", &open, window_flags);

	float windowWidth = ImGui::GetContentRegionAvail().x;
	float buttonWidth = 100; // Define the button width you want
	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center the button

	ImGui::InputInt("Number of Nodes", &newNodesCount);
	ImGui::InputInt("Number of Links", &newLinksCount);

	if (newNodesCount < 0) {
		newNodesCount = 0;
	}
	if (newLinksCount < 0) {
		newLinksCount = 0;
	}

	if (newNodesCount != 0 && newLinksCount > newNodesCount - 1) {
		newLinksCount = newNodesCount - 1;
	}


	if (ImGui::Button("Start", ImVec2(buttonWidth, 0))) {
		DataManager::getInstance().startingNew = false;
	}

	if (!open) {
		DataManager::getInstance().startingNew = false;
	}

	ImGui::End();

	return;
}

char* EditorIMGUI::LoadingUI() {
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize(400, 100); // Desired window size
	ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f,
		(io.DisplaySize.y - windowSize.y) * 0.5f);

	ImGui::SetNextWindowPos(windowPos);
	ImGui::SetNextWindowSize(windowSize);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	bool open = true;
	ImGui::Begin("Loading...", &open, window_flags);

	DataManager::getInstance().ReloadAccessibleFiles();

	DataManager::getInstance().data.SetSelectData(std::move(DataManager::getInstance().fileNames));

	if (ImGui::ComboAutoSelect("Select File", DataManager::getInstance().data)) {
	}
	ImGui::NewLine();
	float windowWidth = ImGui::GetContentRegionAvail().x;
	float buttonWidth = 100; // Define the button width you want
	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center the button

	if (ImGui::Button("Load", ImVec2(buttonWidth, 0))) {
		DataManager::getInstance().loading = false;
	}

	if (!open) {
		std::memset(DataManager::getInstance().data.input, 0, sizeof(DataManager::getInstance().data.input));
		DataManager::getInstance().loading = false;
	}

	ImGui::End();
	
	return DataManager::getInstance().data.input;
}

void EditorIMGUI::ShowAllEntities(Manager& manager, float &m_nodeRadius) {

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));
	std::shared_ptr<OrthoCamera> hud_camera2D = std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));



	ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	float size = 10;

	for (std::size_t group = Manager::groupBackgroundLayer; group != Manager::buttonLabels + 1; group++) {
		std::string s = manager.getGroupName(group);

		if (ImGui::CollapsingHeader(s.c_str())) {
			std::vector<NodeEntity*>& groupVec = manager.getGroup<NodeEntity>(group);

			if ( group == Manager::groupNodes_0 || group == Manager::groupGroupNodes_0 || group == Manager::groupGroupNodes_1) {

				for (auto& entity : groupVec) { // loops 1 time
					Color initialColor = entity->GetComponent<Rectangle_w_Color>().color;
					color = ImVec4(initialColor.r / 255.0f, initialColor.g / 255.0f, initialColor.b / 255.0f, initialColor.a / 255.0f);

					TransformComponent* tr = &entity->GetComponent<TransformComponent>();
					float initialSize[2] = { tr->size.x, tr->size.y };
					size = initialSize[0];
					break;
				}

				if (ImGui::ColorEdit4(("Color##" + s).c_str(), (float*)&color)) {
					std::vector<NodeEntity*>& groupVec = manager.getGroup<NodeEntity>(group);
					Color newColor = {
					   (GLubyte)(color.x * 255),
					   (GLubyte)(color.y * 255),
					   (GLubyte)(color.z * 255),
					   (GLubyte)(color.w * 255)
					};

					for (auto& entity : groupVec) {
						entity->GetComponent<Rectangle_w_Color>().color = newColor;
					}
				}

				if (ImGui::SliderFloat("Node Size", &size, 0, 100)) {
					for (auto& entity : groupVec) {
							entity->GetComponent<TransformComponent>().size.x = size;
							entity->GetComponent<TransformComponent>().size.y = size;
					}
				}

				ImGui::SliderFloat("Border Radius", &m_nodeRadius, 0, 1.0f);

			}

			if (group == Manager::groupLinks_0 || group == Manager::groupGroupLinks_0 || group == Manager::groupGroupLinks_1) {
				for (auto& entity : groupVec) { // loops 1 time
					Color initialColor = entity->GetComponent<Line_w_Color>().src_color;
					color = ImVec4(initialColor.r / 255.0f, initialColor.g / 255.0f, initialColor.b / 255.0f, initialColor.a / 255.0f);

					break;
				}

				if (ImGui::ColorEdit4(("Color##" + s).c_str(), (float*)&color)) {
					std::vector<LinkEntity*>& groupVec = manager.getGroup<LinkEntity>(group);
					Color newColor = {
					   (GLubyte)(color.x * 255),
					   (GLubyte)(color.y * 255),
					   (GLubyte)(color.z * 255),
					   (GLubyte)(color.w * 255)
					};

					for (auto& entity : groupVec) {
						entity->GetComponent<Line_w_Color>().src_color = newColor;
						entity->GetComponent<Line_w_Color>().dest_color = newColor;
					}
				}

				/*if (ImGui::SliderInt("Line Width", &size, 0, 100)) {
					for (auto& entity : groupVec) {
						entity->GetComponent<TransformComponent>().width = size;
						entity->GetComponent<TransformComponent>().height = size;
					}
				}*/
			}

			
			std::string treeNodeLabel = "Entities##" + manager.getGroupName(group);
			if (ImGui::TreeNode(treeNodeLabel.c_str())) {

				for (auto& entity : groupVec) {

					std::string label = "Entity ID: " + std::to_string(entity->getId());

					if (ImGui::TreeNode(label.c_str())) {

						// Create a unique button label
						std::string buttonLabel = "Go to##" + std::to_string(entity->getId());
						if (ImGui::Button(buttonLabel.c_str())) {
							if (entity->hasComponent<TransformComponent>()) {
								main_camera2D->setPosition_X(entity->GetComponent<TransformComponent>().getPosition().x);
								main_camera2D->setPosition_Y(entity->GetComponent<TransformComponent>().getPosition().y);

								main_camera2D->setAimPos(glm::vec3(main_camera2D->eyePos.x, main_camera2D->eyePos.y, main_camera2D->eyePos.z + 1.0f));
							}
						}

						ImGui::TreePop();
					}

				}
				ImGui::TreePop();
			}
			
		}
	}

}

void EditorIMGUI::availableFunctions() {

	if (ImGui::Button("Calculate Degree Of Selected Entities")) {
		_customFunctions.activatedScriptShown = 1;
	}

	if (ImGui::Button("Do Signals")) {
		_customFunctions.activatedScriptShown = 2;
	}

	if (ImGui::Button("Do HeatMap")) {
		_customFunctions.activatedScriptShown = 3;
	}

	if (ImGui::Button("Do CandleStick")) {
		_customFunctions.activatedScriptShown = 4;
	}

	if (ImGui::Button("Show Script Results")) {
		_customFunctions.isScriptResultsOpen = !_customFunctions.isScriptResultsOpen;
	}

}

void EditorIMGUI::SceneViewport(
	const BaseFPSLimiter& baseFPSLimiter, 
	Manager& manager, 
	Framebuffer& m_fb, 
	Framebuffer& m_minimap_fb, 
	ImVec2& storedWindowPos, ImVec2& storedWindowSize
) {

	ImGui::BeginChild("Viewport");

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

	// Render your scene texture
	ImGui::Image(
		reinterpret_cast<void*>(static_cast<uintptr_t>(m_fb._framebufferTexture)),
		viewportPanelSize,
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	storedWindowPos = ImGui::GetWindowPos();
	storedWindowSize = viewportPanelSize;

	// Check if we have valid size and camera
	if (viewportPanelSize.x <= 0 || viewportPanelSize.y <= 0) {
		ImGui::EndChild();
		return;
	}

	// Get camera matrices
	std::shared_ptr<PerspectiveCamera> main_camera2D =
		std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	std::shared_ptr<OrthoCamera> hud_camera2D =
		std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("hud"));
	
	std::shared_ptr<OrthoCamera> minimap_camera2D =
		std::dynamic_pointer_cast<OrthoCamera>(CameraManager::getInstance().getCamera("minimap"));

	
	if (!minimap_camera2D) {
		ImGui::EndChild();
		return;
	}

	// Set up ImGuizmo to render over the image
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	ImGuizmo::SetRect(pos.x, pos.y, viewportPanelSize.x, viewportPanelSize.y);

	// Only enable gizmo when mouse is over the viewport
	bool isHovered = ImGui::IsItemHovered();
	ImGuizmo::Enable(isHovered);
	
	_minimap.setConfig({
	.textureID = m_minimap_fb._framebufferTexture,
	.viewportPos = pos,
	.viewportSize = viewportPanelSize
		});
	_minimap.OnImGuiRender();

	_orientationBox.setConfig({
		.viewportPos = pos,
		.viewportSize = viewportPanelSize
	});
	_orientationBox.OnImGuiRender();

	ImGui::EndChild();
}

void EditorIMGUI::scriptResultsVisualization(Manager& manager, std::vector<std::pair<Entity*, glm::vec3>>& m_selectedEntities) {

	_customFunctions.setSelectedEntities(m_selectedEntities);
	_customFunctions.OnImGuiRender();
}

std::string EditorIMGUI::SceneTabs(std::vector<std::string>& graphNames, std::string& currentActive) {
	float childHeight = 30.0f;
	std::string tabToClose = "";

	ImGui::BeginChild("Scene Tabs", ImVec2(0, childHeight), true, ImGuiWindowFlags_NoScrollbar);
	

	if (ImGui::BeginTabBar("SceneTabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {
		for (size_t i = 0; i < graphNames.size(); ++i) {
			const std::string& name = graphNames[i];

			bool open = true;

			if (ImGui::BeginTabItem(name.c_str(), &open, ImGuiTabItemFlags_None)) {
				currentActive = name;
				ImGui::EndTabItem();
			}
			if (!open) {
				tabToClose = name;

				// If we're closing the currently active tab, switch to another one
				if (currentActive == name && graphNames.size() > 1) {
					// Find a different tab to make active
					for (size_t j = 0; j < graphNames.size(); ++j) {
						if (graphNames[j] != name) {
							currentActive = graphNames[j];
							break;
						}
					}
				}
				else if (graphNames.size() == 1) {
					// If this is the last tab, clear the current active
					currentActive = "";
				}
			}
		}
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 16);
		if (ImGui::ImageButton("play", static_cast<ImTextureID>(static_cast<intptr_t>(TextureManager::getInstance().Get_GLTexture("play-button")->id)), ImVec2(16, 16))) {
			interpolation_running = !interpolation_running;
		}

		ImGui::EndTabBar();
	}
	ImGui::EndChild();
	ImGui::BeginChild("Interpolation Slider", ImVec2(0, 40), true);
	{
		ImGui::Text("Interpolation");
		ImGui::SameLine();
		ImGui::SliderFloat("##interp", &interpolation, 0.0f, 1.0f, "%.2f");

		// Optional: Add tooltip
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Control interpolation");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
		ImGui::Text("Speed");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		ImGui::SliderFloat("##interp_speed", &interpolation_speed, 0.01f, 1.0f, "%.2f");
	}

	ImGui::EndChild();

	if (!tabToClose.empty()) {
		auto it = std::find(graphNames.begin(), graphNames.end(), tabToClose);
		if (it != graphNames.end()) {
			graphNames.erase(it);
		}
	}

	return tabToClose;
}

void EditorIMGUI::ShowFunctionExecutionResults() {
	ImGui::Text("Function Execution Results");
}

void EditorIMGUI::updateIsMouseInSecondColumn() {
	// cant check with the checkIfMouseIsInWidget because it is a child of a window
	ImVec2 columnStartPos = ImGui::GetCursorScreenPos();
	ImVec2 columnSize = ImVec2(ImGui::GetColumnWidth(), ImGui::GetContentRegionAvail().y);

	ImVec2 mousePos = ImGui::GetMousePos();
	isMouseInSecondColumn = (mousePos.x >= columnStartPos.x && mousePos.x <= (columnStartPos.x + columnSize.x) &&
		mousePos.y >= columnStartPos.y && mousePos.y <= (columnStartPos.y + columnSize.y));
}

void EditorIMGUI::showHoveredEntity(Manager& manager, glm::vec2 mousePos, Entity* onHoverEntity)
{
	if (!onHoverEntity) return;

	const float hoveredEntityWindowSize = 220.0f;
	const float windowHeight = 120.0f; // Adjust height based on content

	// Position window near mouse cursor
	ImVec2 hoveredEntityWindowPos = ImVec2(mousePos.x + 10, mousePos.y - windowHeight);

	// Set up ImGuizmo for drawing

	// Get draw list for custom drawing
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	ImGuizmo::SetDrawlist(drawList);
	// Draw background rectangle
	drawList->AddRectFilled(
		hoveredEntityWindowPos,
		ImVec2(hoveredEntityWindowPos.x + hoveredEntityWindowSize, hoveredEntityWindowPos.y + windowHeight),
		IM_COL32(40, 40, 40, 240) // Dark semi-transparent background
	);

	// Draw border
	drawList->AddRect(
		hoveredEntityWindowPos,
		ImVec2(hoveredEntityWindowPos.x + hoveredEntityWindowSize, hoveredEntityWindowPos.y + windowHeight),
		IM_COL32(100, 100, 100, 255),
		3.0f, // Corner rounding
		0,
		2.0f  // Border thickness
	);

	// Draw title
	drawList->AddText(
		ImVec2(hoveredEntityWindowPos.x + 10, hoveredEntityWindowPos.y + 10),
		IM_COL32(255, 255, 255, 255),
		"Hovered Entity"
	);

	// Current text position
	float currentY = hoveredEntityWindowPos.y + 35;
	const float lineHeight = 15.0f;
	const float textX = hoveredEntityWindowPos.x + 10;

	// Display entity information
	Node* node = dynamic_cast<Node*>(onHoverEntity);
	if (node) {
		// Entity ID
		char idText[64];
		sprintf_s(idText, "Id: %d", onHoverEntity->getId());
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), idText);
		currentY += lineHeight;

		// Position
		TransformComponent* tr = &onHoverEntity->GetComponent<TransformComponent>();
		char posText[128];
		sprintf_s(posText, "Position: (%.2f, %.2f)", tr->getPosition().x, tr->getPosition().y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), posText);
		currentY += lineHeight;

		// Size
		char sizeText[128];
		sprintf_s(sizeText, "Size: (%.2f, %.2f)", tr->size.x, tr->size.y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), sizeText);
		currentY += lineHeight;

		// Grid position
		glm::vec3 cellBox = manager.grid->getCell(*onHoverEntity, manager.grid->getGridLevel())->boundingBox_origin;
		char gridText[128];
		sprintf_s(gridText, "Grid: (%.2f, %.2f)", cellBox.x, cellBox.y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), gridText);
	}

	Link* link = dynamic_cast<Link*>(onHoverEntity);
	if (link) {
		char idText[64];
		sprintf_s(idText, "Link Id: %d", onHoverEntity->getId());
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), idText);
	}

	Empty* empty = dynamic_cast<Empty*>(onHoverEntity);
	if (empty) {
		// Entity ID
		char idText[64];
		sprintf_s(idText, "Empty Id: %d", onHoverEntity->getId());
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), idText);
		currentY += lineHeight;

		// Position
		TransformComponent* tr = &onHoverEntity->GetComponent<TransformComponent>();
		char posText[128];
		sprintf_s(posText, "Position: (%.2f, %.2f)", tr->getPosition().x, tr->getPosition().y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), posText);
		currentY += lineHeight;

		// Size
		char sizeText[128];
		sprintf_s(sizeText, "Size: (%.2f, %.2f)", tr->size.x, tr->size.y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), sizeText);
		currentY += lineHeight;

		// Grid position
		glm::vec3 cellBox = manager.grid->getCell(*onHoverEntity, manager.grid->getGridLevel())->boundingBox_origin;
		char gridText[128];
		sprintf_s(gridText, "Grid: (%.2f, %.2f)", cellBox.x, cellBox.y);
		drawList->AddText(ImVec2(textX, currentY), IM_COL32(200, 200, 200, 255), gridText);
	}

}

void EditorIMGUI::ShowEntityComponents(glm::vec2 mousePos, Entity* displayedEntity, Manager& manager)
{

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	if (!displayedEntity) return;


	std::string windowTitle = "Entity";
	Node* node = dynamic_cast<Node*>(displayedEntity);
	Link* link = dynamic_cast<Link*>(displayedEntity);
	Empty* empty = dynamic_cast<Empty*>(displayedEntity);


	if (node) {
		windowTitle = "Node Display";
	}
	else if (link) {
		windowTitle = "Link Display";
	}
	else if (empty) {
		windowTitle = "Empty Display";
	}

	int currentId = displayedEntity->getId();

	if (currentId != _lastEntityDisplayed) {
		ImGui::SetNextWindowPos(ImVec2(mousePos.x, mousePos.y), ImGuiCond_Always);
		_lastEntityDisplayed = currentId;
	}

	if (ImGui::Begin(windowTitle.c_str())) {
		displayedEntity->imgui_print();

		if (node) {
			DataManager::getInstance().data.SetSelectData(std::move(DataManager::getInstance().pollingFileNames));

			if (ImGui::ComboAutoSelect("Select File For Polling", DataManager::getInstance().data)) {
			}
			float buttonWidth = 100;
			if (ImGui::Button("Start Polling Sending Messages", ImVec2(buttonWidth, 0))) {
				std::string selectedFile = DataManager::getInstance().data.input;
				if (!selectedFile.empty()) {
					StartPollingComponent(displayedEntity, selectedFile);
				}
			}
		}
		ImGui::Separator();

		std::unordered_map<std::string, size_t> componentOrder;
		size_t index = 0;
		for (const auto& pair : addComponentMap) {
			componentOrder[pair.first] = index++;
		}

		for (auto& [key, nameVec] : manager.componentNames) {
			std::sort(nameVec.begin(), nameVec.end(), [&](const std::string& a, const std::string& b) {
				return componentOrder[a] < componentOrder[b];
				});
		}

		if (node)
		{
			for (auto& c : manager.componentNames["Component"]) {
				// Checkbox to add/remove component
				bool hasComponent = displayedEntity->hasComponentByName(c);

				if (ImGui::Checkbox(c.c_str(), &hasComponent)) {
					if (hasComponent) {
						AddComponentByName(c, displayedEntity);
					}
					else {
						RemoveComponentByName(c, displayedEntity);
					}
				}

				if (hasComponent) {
					if (ImGui::TreeNode((c + " Properties").c_str())) {
						
						if (c == "SpriteComponent") {
							std::string tempStr = displayedEntity->GetComponent<SpriteComponent>().texture_name;
							getComponentByName(c, displayedEntity)->showGUI();
							ImGui::TreePop();

							if (tempStr != displayedEntity->GetComponent<SpriteComponent>().texture_name) {
								displayedEntity->getManager()->refresh(main_camera2D.get());
							}

						}
						else {
							getComponentByName(c, displayedEntity)->showGUI();
							ImGui::TreePop();
						}
					}
				}
			}
			for (auto& c : manager.componentNames["NodeComponent"]) {
				// Checkbox to add/remove component
				bool hasComponent = displayedEntity->hasComponentByName(c);

				if (ImGui::Checkbox(c.c_str(), &hasComponent)) {
					if (hasComponent) {
						AddComponentByName(c, displayedEntity);
					}
					else {
						RemoveComponentByName(c, displayedEntity);
					}
				}

				if (hasComponent) {
					if (ImGui::TreeNode((c + " Properties").c_str())) {
						getComponentByName(c, displayedEntity)->showGUI();
						ImGui::TreePop();
					}
				}
			}
		}


		else if (link) {
			for (auto& c : manager.componentNames["LinkComponent"]) {
				// Checkbox to add/remove component
				bool hasComponent = displayedEntity->hasComponentByName(c);

				if (ImGui::Checkbox(c.c_str(), &hasComponent)) {
					if (hasComponent) {
						AddComponentByName(c, displayedEntity);
					}
					else {
						RemoveComponentByName(c, displayedEntity);
					}
				}

				if (hasComponent) {
					if (ImGui::TreeNode((c + " Properties").c_str())) {
						getComponentByName(c, displayedEntity)->showGUI();
						ImGui::TreePop();
					}
				}
			}
		}


		else if (empty) {
			for (auto& c : manager.componentNames["Component"]) {
				// Checkbox to add/remove component
				bool hasComponent = displayedEntity->hasComponentByName(c);

				if (ImGui::Checkbox(c.c_str(), &hasComponent)) {
					if (hasComponent) {
						AddComponentByName(c, displayedEntity);
					}
					else {
						RemoveComponentByName(c, displayedEntity);
					}
				}

				if (hasComponent) {
					if (ImGui::TreeNode((c + " Properties").c_str())) {

						if (c == "SpriteComponent") {
							std::string tempStr = displayedEntity->GetComponent<SpriteComponent>().texture_name;
							getComponentByName(c, displayedEntity)->showGUI();
							ImGui::TreePop();

							if (tempStr != displayedEntity->GetComponent<SpriteComponent>().texture_name) {
								displayedEntity->getManager()->refresh(main_camera2D.get());
							}

						}
						else {
							getComponentByName(c, displayedEntity)->showGUI();
							ImGui::TreePop();
						}
					}
				}
			}
		}
	}

	ImGui::End();

}

void EditorIMGUI::ShowSceneControl(glm::vec2 mousePos, Manager& manager)
{
	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	std::string windowTitle = "Scene Manager";

	static int sceneMan_nodeID1 = -1, sceneMan_nodeID2 = -1;
	static std::string errorMessage = "";

	ImGui::SetNextWindowPos(ImVec2(mousePos.x, mousePos.y), ImGuiCond_Always, ImVec2(0, 0));

	if (ImGui::Begin(windowTitle.c_str())) {
		if (ImGui::Button("Create Empty - Box")) {
			auto& empty(manager.addEntityNoId<Empty>());

			glm::vec2 position(0, 0);

			empty.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);

			empty.addComponent<BoxComponent>();

			manager.grid->addEmpty(&empty, manager.grid->getGridLevel());
			empty.addGroup(Manager::groupEmpties);
			manager.aboutTo_updateActiveEntities();
			// todo here we also have choose shape option
		}

		if (ImGui::Button("Create Empty - Plane")) {
			auto& empty(manager.addEntityNoId<Empty>());

			//empty.addGroup(Manager::groupNodes_0);
			// todo here we also have choose shape option
		}

		if (ImGui::Button("Create Empty - Triangle")) {
			auto& empty(manager.addEntityNoId<Empty>());

			//empty.addGroup(Manager::groupNodes_0);
			// todo here we also have choose shape option
		}

		if (ImGui::Button("Create Empty - Sphere")) {
			auto& empty(manager.addEntityNoId<Empty>());

			glm::vec2 position(0, 0);

			empty.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);

			empty.addComponent<SphereComponent>();

			manager.grid->addEmpty(&empty, manager.grid->getGridLevel());
			empty.addGroup(Manager::groupSphereEmpties);
			manager.aboutTo_updateActiveEntities();
		}

		ImGui::Separator();

		if (ImGui::Button("Create Node Entity")) {
			auto& node(manager.addEntityNoId<Node>());

			glm::vec2 position(0, 0);

			node.addComponent<TransformComponent>(position, Layer::action, glm::vec3(10.0f), 1);
			node.addComponent<Rectangle_w_Color>();
			node.GetComponent<Rectangle_w_Color>().color = Color(150, 150, 150, 255);

			node.GetComponent<TransformComponent>().update(0.0f); // update children positions


			manager.grid->addNode(&node, manager.grid->getGridLevel());
			node.addGroup(Manager::groupNodes_0);
			manager.aboutTo_updateActiveEntities();
		}

		ImGui::Separator();
		ImGui::Text("Create Link Entity");
		ImGui::InputInt("Node ID 1", &sceneMan_nodeID1);
		ImGui::InputInt("Node ID 2", &sceneMan_nodeID2);

		if (ImGui::Button("Create Link Entity")) {
			// Check if both nodes exist before creating the link
			if (manager.getEntityFromId(sceneMan_nodeID1) && manager.getEntityFromId(sceneMan_nodeID2)) {
				auto& link(manager.addEntityNoId<Link>(sceneMan_nodeID1, sceneMan_nodeID2));

				
				link.addComponent<Line_w_Color>();

				link.GetComponent<Line_w_Color>().setSrcColor(Color(255, 40, 0, 255));
				link.GetComponent<Line_w_Color>().setDestColor(Color(40, 255, 0, 255));

				link.addComponent<LineFlashAnimatorComponent>();


				link.addGroup(Manager::groupLinks_0);
				manager.grid->addLink(&link, manager.grid->getGridLevel());
				manager.aboutTo_updateActiveEntities();
				errorMessage = ""; // Clear error if successful
			}
			else {
				errorMessage = "Error: One or both node IDs do not exist!";
			}
		}
	}

	ImGui::End();

}

void EditorIMGUI::StartPollingComponent(Entity* entity, const std::string& fileName) {
	if (!entity) return;

	// Attach a polling component to the entity
	if (!entity->hasComponent<PollingComponent>()) {
		entity->addComponent<PollingComponent>();
	}

	entity->GetComponent<PollingComponent>().StartPolling(fileName, 10.0f);

}
