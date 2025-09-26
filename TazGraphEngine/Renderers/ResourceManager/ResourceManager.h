#pragma once

#include <map>

#include <vector>
#include <string>

#include "../GLSLProgram.h"
#include "../GECS/Core/GECS.h"

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

