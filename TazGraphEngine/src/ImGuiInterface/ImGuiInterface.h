#pragma once

#include "../../pch.h"


namespace ImGuiInterface {

	void BeginRender();

	void RenderUI();

	void EndRender();

	bool isMouseOnWidget(const std::string& widgetName);

	void StyleColorsCustom(ImGuiStyle* dst);
};