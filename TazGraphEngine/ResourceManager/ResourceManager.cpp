#include "ResourceManager.h"


void ResourceManager::setupShader(GLSLProgram& shaderProgram, ICamera& camera) {
	shaderProgram.use();
	
	GLint pLocation = shaderProgram.getUniformLocation("projection");
	glm::mat4 cameraMatrix = camera.getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));
}

void ResourceManager::addGLSLProgram(std::string programName)
{	
	GLSLProgram* newGLSLProgram = new GLSLProgram();
	glsl_programs.try_emplace(programName, newGLSLProgram);
}

GLSLProgram* ResourceManager::getGLSLProgram(std::string id)
{
	return glsl_programs[id];
}

void ResourceManager::disposeGLSLPrograms() {
	for (auto& programPair : glsl_programs) {
		programPair.second->dispose();
	}
	glsl_programs.clear();
}
