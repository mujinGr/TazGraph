
#include "./EditorLayoutUtils.h"


	void EditorLayoutUtils::rotateCamera(float& _cameraRotationZ) {
		std::shared_ptr<PerspectiveCamera> main_camera2D = std::dynamic_pointer_cast<PerspectiveCamera>(CameraManager::getInstance().getCamera("main"));

		float angleRad = glm::radians(_cameraRotationZ);
		float radius = 1000.0f;

		float x = cos(angleRad) * radius;
		float y = sin(angleRad) * radius;
		float z = main_camera2D->getPosition().z;

		glm::vec3 newEyePos = glm::vec3(x, y, z);
		main_camera2D->setPosition(newEyePos);

		main_camera2D->setAimPos(glm::vec3(0.0f, 0.0f, 0.0f));
	}

