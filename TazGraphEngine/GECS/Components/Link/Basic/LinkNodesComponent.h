#pragma once

#include "../../../Components.h"

class LinkNodesComponent : public LinkComponent
{
public:
	Line_w_Color* colorLine = nullptr;

	LinkNodesComponent()
	{

	}

	~LinkNodesComponent() {

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

		glm::vec3 fromNodeCenter = entity->getFromNode()->GetComponent<TransformComponent>().getPosition();
		glm::vec3 toNodeCenter = entity->getToNode()->GetComponent<TransformComponent>().getPosition();

		batch.drawLine(v_index, fromNodeCenter, toNodeCenter, colorLine->src_color, colorLine->dest_color, colorLine->width);
	}

};