#pragma once
#include "../../UIElement.h"
#include <Camera2.5D/CameraManager.h>

class FPSCounter : public UIElement {
private:
	const BaseFPSLimiter* baseFPSLimiter = nullptr;

public:
	FPSCounter() = default;
	~FPSCounter() override = default;

	void OnImGuiRender() override
	{
		ImGui::Begin("Performance");
		ImGui::Text("FPS: %f", baseFPSLimiter->fps);
		if (ImPlot::BeginPlot("FPS Plot")) {
			int plot_count = std::min(baseFPSLimiter->fps_history_count,
				baseFPSLimiter->fpsHistoryIndx); // Ensuring we do not read out of bounds
			int plot_offset = std::max(0,
				baseFPSLimiter->fpsHistoryIndx - baseFPSLimiter->fps_history_count); // Ensure a positive offset

			ImPlot::SetupAxesLimits(0, 100, 0, 200);

			ImPlot::PlotLine("FPS", &baseFPSLimiter->fpsHistory[0], plot_count);

			ImPlot::EndPlot();
		}
		ImGui::End();
	};

	void setLimiter(const BaseFPSLimiter& m_baseFPSLimiter) {
		baseFPSLimiter = &m_baseFPSLimiter;
	}

};