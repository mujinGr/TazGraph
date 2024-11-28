#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>

#include "../ConsoleLogger.h"
#include "../AssetManager/AssetManager.h"

class GLSLProgram {
public:
	GLSLProgram();
	~GLSLProgram();

	void compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

	void compileShadersFromSource(const char* vertexSource, const char* fragmentSource);

	void linkShaders();

private:
	GLuint _programID;

	GLuint _vertexShaderID;
	GLuint _fragmentShaderID;

	void compileShader(const char* source, const std::string& name, GLuint id);
};
