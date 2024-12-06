#pragma once

#include <iostream>
#include <string>
#include "GL/glew.h"

#include <vector>
#include <fstream>

#include "TextureManager/TextureManager.h"
#include "ConsoleLogger.h"

class GLSLProgram {
public:
	GLSLProgram() : _programID(0), _vertexShaderID(0), _fragmentShaderID(0), _numAttributes(0)
	{

	}

	~GLSLProgram() {

	}

	void compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath) {

		std::vector<unsigned char> vertSourceVec;
		std::vector<unsigned char> fragSourceVec;

		TextureManager::readFileToBuffer(vertexShaderFilePath.c_str(), vertSourceVec);
		TextureManager::readFileToBuffer(fragmentShaderFilePath.c_str(), fragSourceVec);

		std::string vertSource(vertSourceVec.begin(), vertSourceVec.end());
		std::string fragSource(fragSourceVec.begin(), fragSourceVec.end());

		compileShadersFromSource(vertSource.c_str(), fragSource.c_str());
	}

	void compileShadersFromSource(const char* vertexSource, const char* fragmentSource) {

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		_programID = glCreateProgram();

		_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		if (_vertexShaderID == 0) {
			TazGraphEngine::ConsoleLogger::error("Vertex Shader Failed to create!");
		}

		_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (_fragmentShaderID == 0) {
			TazGraphEngine::ConsoleLogger::error("Fragment Shader Failed to create!");
		}

		compileShader(vertexSource, "vertex Shader", _vertexShaderID);
		compileShader(fragmentSource, "fragment Shader", _fragmentShaderID);
	}

	void linkShaders() {
		// Attach our shaders to our program
		glAttachShader(_programID, _vertexShaderID);
		glAttachShader(_programID, _fragmentShaderID);

		// Link our program
		glLinkProgram(_programID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(_programID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(_programID, maxLength, &maxLength, &errorLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(_programID);
			// Don't leak shaders either.
			glDeleteShader(_vertexShaderID);
			glDeleteShader(_fragmentShaderID);

			std::printf("%s\n", &(errorLog[0]));
			TazGraphEngine::ConsoleLogger::error("Shaders failed to link");
		}

		// Always detach shaders after a successful link.
		glDetachShader(_programID, _vertexShaderID);
		glDetachShader(_programID, _fragmentShaderID);
		glDeleteShader(_vertexShaderID);
		glDeleteShader(_fragmentShaderID);
	}

	void addAttribute(const std::string& attributeName) {
		glBindAttribLocation(_programID, _numAttributes++, attributeName.c_str());
	}

	GLint getUniformLocation(const std::string& uniformName) {
		GLint location = glGetUniformLocation(_programID, uniformName.c_str());

		if (location == GL_INVALID_INDEX) {
			TazGraphEngine::ConsoleLogger::error("Uniform " + uniformName + " not found in shader!");
		}
		return location;
	}

	void use() {
		glUseProgram(_programID);
		for (int i = 0; i < _numAttributes; i++) {
			glEnableVertexAttribArray(i);
		}
	}

	void unuse() {
		glUseProgram(0);
		for (int i = 0; i < _numAttributes; i++) {
			glDisableVertexAttribArray(i);
		}
	}

	void dispose() {
		if(_programID != 0) glDeleteProgram(_programID);
	}

private:
	GLuint _programID;

	GLuint _vertexShaderID;
	GLuint _fragmentShaderID;

	int _numAttributes;

	void compileShader(const char* source, const std::string& name, GLuint id) {

		glShaderSource(id, 1, &source, nullptr); //1 for number of strings

		glCompileShader(id);

		GLint success = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteShader(id); // Don't leak the shader.

			std::printf("%s\n", &(errorLog[0]));
			std::cout << "Shader " + name + " failed to compile" << std::endl;

		}
	}
};