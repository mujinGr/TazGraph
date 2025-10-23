#pragma once

#include "../../../Components.h"

class LinePositionsComponent : public LinkComponent
{
public:
	Line_w_Color* colorLine = nullptr;
	glm::vec3 fromPos = glm::vec3(0);
	glm::vec3 toPos = glm::vec3(0);

	LinePositionsComponent()
	{

	}

	~LinePositionsComponent() {

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

		batch.drawLine(v_index, fromPos, toPos, colorLine->src_color, colorLine->dest_color, colorLine->width);
	}

};