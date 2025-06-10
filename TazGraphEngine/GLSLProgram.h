#pragma once

#include <iostream>
#include <string>
#include "GL/glew.h"

#include <vector>
#include <fstream>

#include "./Vertex.h"

#include "TextureManager/TextureManager.h"
#include "ConsoleLogger.h"

#define LINE_OFFSET 2
#define SQUARE_OFFSET 4

#define TRIANGLE_VERTICES 3
#define QUAD_INDICES 6
#define BOX_OFFSET 8

#define TOTAL_MESHES 3

#define TRIANGLE_MESH_IDX 0
#define RECTANGLE_MESH_IDX 1
#define BOX_MESH_IDX 2


constexpr int INDICES_LINE_OFFSET = LINE_OFFSET;
constexpr int INDICES_SQUARE_OFFSET = 2 * SQUARE_OFFSET;
constexpr int INDICES_BOX_OFFSET = 3 * BOX_OFFSET;

constexpr int ARRAY_BOX_OFFSET = 36;


static Position triangleVertices[3] = {
	{  0.0f,  0.5f, 0.0f }, // Top
	{ -0.5f, -0.5f, 0.0f }, // Bottom Left
	{  0.5f, -0.5f, 0.0f }  // Bottom Right
};

static GLuint triangleIndices[3] = {
	0, 1, 2
};

static Position quadVertices[4] = {
	{	-0.5f,  0.5f,	0.0f	},
	{	-0.5f,  -0.5f,	0.0f	},
	{	0.5f,   -0.5f,	0.0f	},
	{	0.5f,   0.5f,	0.0f	}
};

static UV uv_quadVertices[4] = {
	{0.0f, 0.0f},
	{1.0f, 0.0f},
	{1.0f, 1.0f},
	{0.0f, 1.0f}
};

static TextureVertex tex_quadVertices[4] = {
	{ glm::vec3(-0.5f,  0.5f, 0.0f),  glm::vec2(0.0f, 1.0f) }, // top-left
	{ glm::vec3(-0.5f, -0.5f, 0.0f),  glm::vec2(0.0f, 0.0f) }, // bottom-left
	{ glm::vec3(0.5f, -0.5f, 0.0f),  glm::vec2(1.0f, 0.0f) }, // bottom-right
	{ glm::vec3(0.5f,  0.5f, 0.0f),  glm::vec2(1.0f, 1.0f) }  // top-right
};

static GLuint quadIndices[6] = {
	0, 1, 2,
	2, 3, 0
};

static Position cubeVertices[8] = {
	{ -0.5f, -0.5f, -0.5f }, 
	{  0.5f, -0.5f, -0.5f }, 
	{  0.5f,  0.5f, -0.5f }, 
	{ -0.5f,  0.5f, -0.5f }, 
	{ -0.5f, -0.5f,  0.5f }, 
	{  0.5f, -0.5f,  0.5f },
	{  0.5f,  0.5f,  0.5f }, 
	{ -0.5f,  0.5f,  0.5f }  
};

static LightVertex light_cubeVertices[24] = {
	// Front face
	{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f) },
	{ glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, 0.0f, -1.0f) },
	{ glm::vec3(0.5f, 0.5f, -0.5f),   glm::vec3(0.0f, 0.0f, -1.0f) },
	{ glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(0.0f, 0.0f, -1.0f) },

	// Back face
	{ glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(0.5f, -0.5f, 0.5f),   glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(0.0f, 0.0f, 1.0f) },

	// Left face
	{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f) },
	{ glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(-1.0f, 0.0f, 0.0f) },
	{ glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(-1.0f, 0.0f, 0.0f) },
	{ glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(-1.0f, 0.0f, 0.0f) },

	// Right face
	{ glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.5f, -0.5f, 0.5f),   glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.5f, 0.5f, -0.5f),   glm::vec3(1.0f, 0.0f, 0.0f) },

	// Bottom face
	{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f) },
	{ glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, -1.0f, 0.0f) },
	{ glm::vec3(0.5f, -0.5f, 0.5f),   glm::vec3(0.0f, -1.0f, 0.0f) },
	{ glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(0.0f, -1.0f, 0.0f) },

	// Top face
	{ glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(0.5f, 0.5f, -0.5f),   glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(0.0f, 1.0f, 0.0f) }
};

static GLuint cubeIndices[36] = {
	 0, 1, 2,
	2, 3, 0,

	// Back face (vertices 4-7)
	4, 5, 6,
	6, 7, 4,

	// Left face (vertices 8-11)
	8, 9,10,
	10,11,8,

	// Right face (vertices 12-15)
	12,13,14,
	14,15,12,

	// Bottom face (vertices 16-19)
	16,17,18,
	18,19,16,

	// Top face (vertices 20-23)
	20,21,22,
	22,23,20
};


struct InstanceData {

	InstanceData() {}
	InstanceData(glm::vec3 mSize, Position mBodyCenter, Rotation mRotation) :
		size(mSize),
		bodyCenter(mBodyCenter),
		rotation(mRotation)
	{
	}

	InstanceData(glm::vec2 mSize, Position mBodyCenter, Rotation mRotation) :
		size(glm::vec3(mSize, 0.0f)),
		bodyCenter(mBodyCenter),
		rotation(mRotation)
	{
	}

	~InstanceData() {}

	Size size = glm::vec3(0.0f);
	Position bodyCenter = glm::vec3(0.0f);
	Rotation rotation = glm::vec3(0.0f);
};


struct ColorInstanceData : InstanceData {

	ColorInstanceData() {}
	ColorInstanceData(glm::vec3 mSize, Position mBodyCenter, Rotation mRotation, Color mColor) : InstanceData(mSize, mBodyCenter, mRotation), color(mColor) {
	}
	ColorInstanceData(glm::vec2 mSize, Position mBodyCenter, Rotation mRotation, Color mColor) : InstanceData(mSize, mBodyCenter, mRotation), color(mColor) {
	}

	~ColorInstanceData() {};

	Color color = Color(255, 255, 255, 255);
};

struct TextureInstanceData : InstanceData {

	TextureInstanceData() {}
	TextureInstanceData(glm::vec3 mSize, Position mBodyCenter, Rotation mRotation, GLuint Texture) : InstanceData(mSize, mBodyCenter, mRotation), texture(Texture) {
	}
	TextureInstanceData(glm::vec2 mSize, Position mBodyCenter, Rotation mRotation, GLuint Texture) : InstanceData(mSize, mBodyCenter, mRotation), texture(Texture) {
	}

	~TextureInstanceData() {};

	GLuint texture = 0;
	UV uv = glm::vec2(0.0f);
};

struct MeshRenderer {
	size_t meshIndices = 0;

	std::vector<InstanceData> instances;

	GLuint vao;

	GLuint vbo; //for static draws
	GLuint ibo;
};

struct ColorMeshRenderer {
	size_t meshIndices = 0;

	std::vector<ColorInstanceData> instances;

	GLuint vao;

	GLuint vbo; //for static draws
	GLuint ibo;
};

struct TextureMeshRenderer {
	size_t meshIndices = 0;

	std::vector<TextureInstanceData> instances;

	GLuint vao;

	GLuint vbo; //for static draws
	GLuint ibo;
};

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

	GLuint getProgramID() {
		return _programID;
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