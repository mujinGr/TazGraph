#ifndef GRAPH_H
#define GRAPH_H

#include <TazGraphEngine.h>

#include <GL/glew.h>
#undef main

#include "../../AssetManager/AssetManager.h"
#include "../../EditorIMGUI/GraphEditorLayer/GraphEditorLayer.h"
#include "../../AssetManager/PythonEngineUtil.h"

class Map;
class SceneManager;
class ColliderComponent;
class TransformComponent;



class Graph : public IScene {

public:
	Graph();
	Graph(bool m_usePython, std::array<float, 4> bg, bool m_useGrid) :
		usePython(m_usePython)
	{
		std::copy(bg.begin(), bg.end(), backgroundColor);
		showGrid = m_useGrid;
		last_showGrid = !m_useGrid;
		_sceneIndex = SCENE_INDEX_GRAPHPLAY;
	};
	~Graph();


	virtual int getNextSceneIndex() const override;

	virtual int getPreviousSceneIndex() const override;

	virtual void build() override;

	virtual void destroy() override;

	virtual void onEntry() override;

	virtual void onExit() override;

	virtual void update(float deltaTime) override;

	virtual void prepareDraw(int index) override;
	virtual void renderDraw(int index) override;

	void minimapPrepareDraw(int index);
	void minimapRenderDraw(int index);

	virtual void SwapBufferDraw() override;

	virtual void BeginRender() override;
	virtual void updateUI(float deltaTime) override;
	virtual void drawUI() override;
	virtual void EndRender() override;


	GraphLoader* graphLoader = nullptr;
	//std::unique_ptr<Grid> grid;

private:

	void clearSelectedEntities();

	std::vector<Cell*> traversedCellsFromRay(glm::vec3 rayOrigin,
		glm::vec3 rayDirection,
		float maxDistance);

	void selectEntityFromRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, int activateMode);

	bool setManager(std::string m_managerName) override;

	void checkInput();
	void performFrustumSelection();
	bool onPauseGraph();

	std::vector<SelectedInfo> _selectedEntities;

	Entity* _displayedEntity = nullptr;
	bool _sceneManagerActive = false;
	Entity* _onHoverEntity = nullptr;

	Uint32 _holdStartTime = 0;

	bool _isDraggingSelectionBox = false;

	glm::vec2 _selectionStartPos = glm::vec2(0);
	glm::vec2 _selectionCurrentPos = glm::vec2(0);

	glm::vec2 _selectionWindowStartPos = glm::vec2(0);
	glm::vec2 _selectionWindowCurrentPos = glm::vec2(0);

	int _nextSceneIndex = SCENE_INDEX_GRAPHPLAY;
	int _prevSceneIndex = SCENE_INDEX_MAIN_MENU;

	const float SCALE_SPEED = 0.1f;
	bool _firstLoop = true;

	bool usePython = false;

	GraphEditorLayer _graphEditorLayer = GraphEditorLayer(usePython);

	Framebuffer _viewportFramebuffer;
	Framebuffer _minimapFramebuffer;

	ImVec2 _viewportPos;
	ImVec2 _viewportSize;

	glm::vec2 _viewportMousePosition = { 0.f,0.f };

	// Minimap stuff
	int currentX = 0, currentY = 0, currentZ = 0;
	bool processingComplete = false;
	bool needsRefresh = true;

	template<typename EntityType>
	void selectEntitiesInFrustum(int groupId, const SelectionFrustum& frustum);

	float cameraVelocityX = 0.0f;
	float cameraVelocityY = 0.0f;
	float cameraVelocityZ = 0.0f;
	float cameraAcceleration = 10.0f;
	float cameraMaxVelocity = 200.0f;
	float cameraFriction = 0.9f;

	PythonEngineUtil peu;
};


#endif