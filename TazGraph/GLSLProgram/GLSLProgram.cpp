#include "GLSLProgram.h"
#include "../ConsoleLogger/ConsoleLogger.h"

#include <fstream>

GLSLProgram::GLSLProgram() : _programID(0), _vertexShaderID(0), _fragmentShaderID(0), _numAttributes(0)
{

}

GLSLProgram::~GLSLProgram()
{

}



void GLSLProgram::compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
	_programID = glCreateProgram();

	std::vector<unsigned char> vertSourceVec;
	std::vector<unsigned char> fragSourceVec;

	AssetManager::readFileToBuffer(vertexShaderFilePath.c_str(), vertSourceVec);
	AssetManager::readFileToBuffer(fragmentShaderFilePath.c_str(), fragSourceVec);

	std::string vertSource(vertSourceVec.begin(), vertSourceVec.end());
	std::string fragSource(fragSourceVec.begin(), fragSourceVec.end());

	compileShadersFromSource(vertSource.c_str(), fragSource.c_str());
}

void GLSLProgram::compileShadersFromSource(const char* vertexSource, const char* fragmentSource) {


	_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	if (_vertexShaderID == 0) {
		nmConsoleLogger::error("Vertex Shader Failed to create!");
	}

	_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	if (_fragmentShaderID == 0) {
		nmConsoleLogger::error("Fragment Shader Failed to create!");
	}

	compileShader(vertexSource, "vertex Shader", _vertexShaderID);
	compileShader(fragmentSource, "fragment Shader", _fragmentShaderID);
}

void GLSLProgram::linkShaders()
{

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
		nmConsoleLogger::error("Shaders failed to link");
	}

	// Always detach shaders after a successful link.
	glDetachShader(_programID, _vertexShaderID);
	glDetachShader(_programID, _fragmentShaderID);
	glDeleteShader(_vertexShaderID);
	glDeleteShader(_fragmentShaderID);
}

void GLSLProgram::addAttribute(const std::string& attributeName)
{
	glBindAttribLocation(_programID, _numAttributes++, attributeName.c_str());
}

void GLSLProgram::use()
{
	glUseProgram(_programID);
	for (int i = 0; i < _numAttributes; i++) {
		glEnableVertexAttribArray(i);
	}
}

void GLSLProgram::unuse()
{
	glUseProgram(0);
	for (int i = 0; i < _numAttributes; i++) {
		glDisableVertexAttribArray(i);
	}
}

void GLSLProgram::dispose()
{
	if (_programID != 0) glDeleteProgram(_programID);
}

void GLSLProgram::compileShader(const char* source, const std::string& name, GLuint id)
{
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
		nmConsoleLogger::error("Shader " + name + " failed to compile");
	}
}
