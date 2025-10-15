#pragma once

#include "../../UIElement.h"

#include "../../../AssetManager/AssetManager.h"

struct GraphLeftConfig {
	IScene* scene;
	glm::vec2 sceneMouseCoords;
	Entity* displayedEntity = nullptr;
};

class GraphLeftPanel : public UIElement
{
private:

	enum class LayoutState {
		Idle,
		ProcessingCircular,
		ProcessingCluster
	};
	LayoutState _currentLayoutState = LayoutState::Idle;

	bool _clusterLayoutEnabled = false;
	bool _circularLayoutProcessing = false;

	bool _clusterLayout = false;
	GraphLeftConfig config;

	int last_activeLayout = 0;
	int activeLayout = 0;
public:
	void setConfig(const GraphLeftConfig& cfg) { config = cfg; }

	void update(float deltaTime) override;

	void OnImGuiRender() override;
	void ChooseLayoutPanel();
	void displayChildrenRecursive(Entity* entity, int depth);
};