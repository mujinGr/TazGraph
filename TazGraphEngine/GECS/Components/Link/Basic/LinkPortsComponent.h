#pragma once


#include "../../../Components.h"

class LinkPortsComponent : public LinkComponent
{
public:
	Line_w_Color* colorLine = nullptr;

	LinkPortsComponent()
	{

	}

	~LinkPortsComponent() {

	}

	void init() override
	{
		if (!entity->hasComponent<Line_w_Color>())
		{
			entity->addComponent<Line_w_Color>();
		}
		colorLine = &entity->GetComponent<Line_w_Color>();
	}

	void draw(size_t v_index, LineRenderer& batch, TazGraphEngine::Window& window) {
		//float tempScreenScale = window.getScale();

		//NodeEntity* fromNode = entity->getFromNode();
		//NodeEntity* toNode = entity->getToNode();

		//Entity* fromPortEntity = entity->getManager()->getEntityFromId(fromNode->children[entity->fromPort]);
		//Entity* toPortEntity = entity->getManager()->getEntityFromId(toNode->children[entity->toPort]);

		//if ((entity->fromSlotIndex >= fromPortEntity->children.size())
		//	|| (entity->toSlotIndex >= toPortEntity->children.size())) {
		//	return;
		//}
		//glm::vec3 fromConnectionPoint = entity->getManager()->getEntityFromId(fromPortEntity->children[entity->fromSlotIndex])->GetComponent<TransformComponent>().getPosition();
		//glm::vec3 toConnectionPoint = entity->getManager()->getEntityFromId(toPortEntity->children[entity->toSlotIndex])->GetComponent<TransformComponent>().getPosition();

		//batch.drawLine(v_index, fromConnectionPoint, toConnectionPoint, colorLine->src_color, colorLine->dest_color, colorLine->width);
	}
};