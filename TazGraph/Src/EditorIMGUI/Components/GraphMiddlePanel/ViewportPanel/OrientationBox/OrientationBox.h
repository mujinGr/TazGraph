#pragma once

#include "../../../../UIElement.h"

#include "TazGraphEngine.h"

struct OrientationBoxConfig {
	ImVec2 viewportPos;
	ImVec2 viewportSize;
};


class OrientationBox : public UIElement {
public:
	void OnImGuiRender() override;
	void UpdateCameraFromMatrix();

	void setConfig(const OrientationBoxConfig& cfg);

private:
	glm::mat4 _boxViewMatrix = glm::mat4(1.0f);
	OrientationBoxConfig config;

};