#include "CameraPanel.h"

void CameraPanel::update(float deltaTime)
{
	// subComponents update
}

void CameraPanel::OnImGuiRender()
{
	ImGui::Text("Camera:");

	std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

	ImGui::Text("Rect: {x: %f, y: %f, w: %f, h: %f}", main_camera2D->getCameraRect().x, main_camera2D->getCameraRect().y, main_camera2D->getCameraRect().w, main_camera2D->getCameraRect().h);

	if (ImGui::SliderFloat3("Eye Position", &main_camera2D->eyePos[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}
	if (ImGui::SliderFloat3("Aim Position", &main_camera2D->aimPos[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}
	if (ImGui::SliderFloat3("Up Direction", &main_camera2D->upDir[0], -1000.0f, 1000.0f)) {
		main_camera2D->setCameraMatrix(glm::lookAt(main_camera2D->eyePos, main_camera2D->aimPos, main_camera2D->upDir));
	}

	const char* viewModeNames[] = { "Y-Up", "Z-Up" };

	if (ImGui::Combo("Orientation", &_currentOrientationIndex, viewModeNames, IM_ARRAYSIZE(viewModeNames))) {
		ViewMode newMode = static_cast<ViewMode>(_currentOrientationIndex);
		main_camera2D->currentViewMode = newMode;

		glm::vec3 eyePos = main_camera2D->getPosition();

		if (newMode == ViewMode::Y_UP) {
			//main_camera2D->upDir = glm::vec3(0, -1, 0);
			main_camera2D->setAimPos(glm::vec3(eyePos.x, eyePos.y, eyePos.z + 1.0f));
		}
		else if (newMode == ViewMode::Z_UP) {
			//main_camera2D->upDir = glm::vec3(0, 0, -1);
			main_camera2D->setAimPos(glm::vec3(eyePos.x, eyePos.y + 1.0f, eyePos.z));
		}

	}

	if (ImGui::Button("Reset")) {
		main_camera2D->resetCameraPosition();
	}
}
