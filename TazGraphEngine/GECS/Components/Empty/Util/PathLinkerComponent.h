#pragma once

#include "../../../Components.h"

class PathLinkerComponent : public Component
{
public:
	std::vector<LinkEntity*> innerLinks;
	std::vector<LinkEntity*> pathLinks;

	float width = 1.0f;   // default width
	Color color = Color(0, 0, 0, 255); // default black

	TransformComponent* transform = nullptr;

	PathLinkerComponent()
	{

	}

	~PathLinkerComponent() {

	}

	void init() override {
		transform = &entity->GetComponent<TransformComponent>();
	}

	void update(float deltaTime) override {
		
	}

	void draw(size_t v_index, PlaneColorRenderer& batch, TazGraphEngine::Window& window) {

	}

	// Add a link to the path
	void addLink(LinkEntity* link) {
		if (link && std::find(pathLinks.begin(), pathLinks.end(), link) == pathLinks.end()) {
			link->GetComponent<Line_w_Color>().setSrcColor(color);
			link->GetComponent<Line_w_Color>().setDestColor(color);
			link->GetComponent<Line_w_Color>().width = width;
			pathLinks.push_back(link);
		}
	}

	// Remove a link from the path
	void removeLink(LinkEntity* link) {
		pathLinks.erase(std::remove(pathLinks.begin(), pathLinks.end(), link), pathLinks.end());
	}

	std::string GetComponentName() override {
		return "PathLinkerComponent";
	}

	void showGUI() override {
		ImGui::Separator();
	}

	void createInnerLinks() {
		for (size_t i = 1; i < pathLinks.size(); i++) {
			LinkEntity* prevLink = pathLinks[i - 1];
			LinkEntity* currLink = pathLinks[i];

			// Validate nodes and ports for prevLink's toPort and currLink's fromPort
			NodeEntity* prevToNode = prevLink->getToNode();
			NodeEntity* currFromNode = currLink->getFromNode();

			if (prevToNode != currFromNode)
				TazGraphEngine::ConsoleLogger::error("Should be same node to create innerPaths!");

			if (!prevToNode || !currFromNode)
				continue;

			int prevToPortIndex = prevLink->toPort;
			int currFromPortIndex = currLink->fromPort;

			if (prevToPortIndex < 0 || currFromPortIndex < 0)
				continue;

			// Create the inner link connecting prevLink's toPort to currLink's fromPort
			auto& newInnerLink = entity->getManager()->addEntity<Link>(
				prevToNode, currFromNode,
				prevToPortIndex, currFromPortIndex,
				prevLink->toSlotIndex, currLink->fromSlotIndex
			);
			newInnerLink.addGroup(Manager::groupPathInnerLinks);  // or a dedicated group for inner links

			newInnerLink.addComponent<Line_w_Color>();

			innerLinks.push_back(&newInnerLink);

			entity->getManager()->grid->addLink(&newInnerLink, entity->getManager()->grid->getGridLevel());
		}
	}

	void removeInnerLinks() {
		for (auto* innerLink : innerLinks) {
			if (!innerLink) continue;
			
			innerLink->destroy();
		}
		innerLinks.clear();
	}
};