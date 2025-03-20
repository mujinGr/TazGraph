#include "ResourceManager.h"


void ResourceManager::setupShader_Texture(GLSLProgram& shaderProgram, const std::string& textureName) {
	GLint textureLocation = shaderProgram.getUniformLocation("texture_sampler");
	glUniform1i(textureLocation, 0);
}

void ResourceManager::setupShader(GLSLProgram& shaderProgram, const std::string& textureName, ICamera& camera) {
	shaderProgram.use();
	if (!textureName.empty()) {
		setupShader_Texture(shaderProgram, textureName);
	}
	GLint pLocation = shaderProgram.getUniformLocation("projection");
	glm::mat4 cameraMatrix = camera.getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));
}

void ResourceManager::setupShader(GLSLProgram& shaderProgram, ICamera& camera) {
	setupShader(shaderProgram, "", camera);
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
