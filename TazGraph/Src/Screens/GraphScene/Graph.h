#ifndef GRAPH_H
#define GRAPH_H

#include <AppScene/IScene.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <GLSLProgram.h>
#include <ResourceManager/ResourceManager.h>
#undef main
#include <iostream>
#include <vector>
#include "Camera2.5D/PerspectiveCamera.h"
#include <Renderers/FrameBuffer/Framebuffer.h>
#include <Renderers/PlaneRenderers/PlaneModelRenderer/PlaneModelRenderer.h>
#include <InputManager/InputManager.h>
#include <BaseFPSLimiter/BaseFPSLimiter.h>
//#include "SpriteFont/SpriteFont.h"
#include "Window/Window.h"
#include "TextureManager/TextureManager.h"


#include "AppScene/ScreenIndices.h"

#include <ImGuiInterface/ImGuiInterface.h>

#include <chrono>
#include <thread>
#include "../../EditorIMGUI/Components/MenuDropdown/MenuDropdown.h"
#include "../../EditorIMGUI/Components/FPSCounter.h"
#include "../../EditorIMGUI/Components/GraphTopBar/GraphTopBar.h"
#include "../../EditorIMGUI/Components/GraphLeftPanel/GraphLeftPanel.h"
#include "../../EditorIMGUI/Components/ViewportPanel/ViewportPanel.h"
#include "../../EditorIMGUI/Components/GraphRightPanel/GraphRightPanel.h"
#include "../../AssetManager/AssetManager.h"
#include "../../EditorIMGUI/Components/SceneControl/SceneControl.h"
#include "../../EditorIMGUI/Components/EntityComponentsControl/EntityComponentsControl.h"
#include "../../EditorIMGUI/Components/HoverEntityPanel/HoverEntityPanel.h"

class Map;
class SceneManager;
class ColliderComponent;
class TransformComponent;



#define ON_HOVER 0
#define CTRLD_LEFT_CLICK -1

class Graph : public IScene {

public:
	Graph(TazGraphEngine::Window* window);
	~Graph();


	virtual int getNextSceneIndex() const override;

	virtual int getPreviousSceneIndex() const override;

	virtual void build() override;

	virtual void destroy() override;

	virtual void onEntry() override;

	virtual void onExit() override;

	virtual void update(float deltaTime) override;

	virtual void draw() override;

	virtual void BeginRender() override;
	virtual void updateUI(float deltaTime) override;
	virtual void drawUI() override;
	virtual void EndRender() override;


	/////////////////////////
	void renderBatch(const std::vector<LinkEntity*>& entities, LineRenderer& batch);
	void renderBatch(const std::vector<EmptyEntity*>& entities, PlaneColorRenderer& batch);
	void renderBatch(const std::vector<NodeEntity*>& entities, PlaneColorRenderer& batch);
	void renderBatch(const std::vector<EmptyEntity*>& entities, PlaneModelRenderer& batch);
	void renderBatch(const std::vector<NodeEntity*>& entities, PlaneModelRenderer& batch);
	void renderBatch(const std::vector<EmptyEntity*>& entities, LightRenderer& batch);
	void drawHUD(const std::vector<NodeEntity*>& entities);

	/////////////////////////

	Map* map = nullptr;
	//std::unique_ptr<Grid> grid;

	static TazGraphEngine::Window* _window;


private:
	float _backgroundColor[4] = { 0.407f,0.384f,0.356f, 1.0f };
	float _minimap_backgroundColor[4] = { 1.f,0.f,0.f, 1.0f };

	std::vector<Cell*> traversedCellsFromRay(glm::vec3 rayOrigin,
		glm::vec3 rayDirection,
		float maxDistance);

	void selectEntityFromRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, int activateMode);

	bool setManager(std::string m_managerName);

	void checkInput();
	bool onPauseGraph();

	PlaneModelRenderer _PlaneModelRenderer;
	PlaneModelRenderer _hudPlaneModelRenderer;
	PlaneColorRenderer _PlaneColorRenderer;
	LineRenderer _LineRenderer;
	LightRenderer _LightRenderer;

	ResourceManager _resourceManager;

	std::vector<std::pair<Entity*, glm::vec3>> _selectedEntities;
	Entity* _displayedEntity = nullptr;
	bool _sceneManagerActive = false;
	Entity* _onHoverEntity = nullptr;
	
	int _nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
	int _prevSceneIndex = SCENE_INDEX_MAIN_MENU;

	const float SCALE_SPEED = 0.1f;
	bool _firstLoop = true;

	MenuDropdownPanel _menuDropdown;
	FPSCounter _fpsCounter;
	GraphTopBar _topBar;
	GraphLeftPanel _graphLeftPanel;
	ViewportPanel _viewportPanel;
	GraphRightPanel _graphRightPanel;
	SceneControlPanel _sceneControl;
	EntityComponentsControlPanel _entityComponentController;
	HoverEntityPanel _hoverEntityPanel;

	Framebuffer _framebuffer;
	Framebuffer _minimapFramebuffer;

	ImVec2 _windowPos;
	ImVec2 _windowSize;

	glm::vec2 _sceneMousePosition = {0.f,0.f};
	glm::vec2 _savedMainViewportMousePosition = { 0.f,0.f };

};


#endif