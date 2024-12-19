#pragma once

#include <map>

#include <vector>
#include <string>

#include "../GLSLProgram.h"
#include "../GOS/GOS.h"

class ResourceManager
{
public:
	void setupShader_Texture(GLSLProgram& shaderProgram, const std::string& textureName);

	/////////////////////////
	void setupShader(GLSLProgram& shaderProgram, const std::string& textureName, ICamera& camera);
	
	void addGLSLProgram(std::string programName);
	GLSLProgram* getGLSLProgram(std::string id);
private:
	std::map<std::string, GLSLProgram*> glsl_programs;
};

