#pragma once

#include "../../../pch.h"

#include "../../GLSLProgram.h"
#include "../../Camera2.5D/ICamera.h"

class ResourceManager
{
public:
	/////////////////////////

	void setupShader(GLSLProgram& shaderProgram, ICamera& camera);

	void addGLSLProgram(std::string programName);
	GLSLProgram* getGLSLProgram(std::string id);

	void disposeGLSLPrograms();
private:
	std::map<std::string, GLSLProgram*> glsl_programs;
};

