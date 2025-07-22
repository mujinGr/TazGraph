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

#define TOTAL_MESHES 4

#define TRIANGLE_MESH_IDX 0
#define RECTANGLE_MESH_IDX 1
#define BOX_MESH_IDX 2
#define SPHERE_MESH_IDX 3

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


static void generateSphereMesh(std::vector<Position>& vertices, std::vector<GLuint>& indices,
	float radius = 1.0f, unsigned int sectorCount = 36, unsigned int stackCount = 18) {
	const float PI = 3.14159265359f;

	vertices.clear();
	indices.clear();

	for (unsigned int i = 0; i <= stackCount; ++i) {
		float stackAngle = PI / 2.0f - i * PI / stackCount;  // from pi/2 to -pi/2
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		for (unsigned int j = 0; j <= sectorCount; ++j) {
			float sectorAngle = j * 2.0f * PI / sectorCount;

			float x = xy * cosf(sectorAngle);
			float y = xy * sinf(sectorAngle);
			glm::vec3 pos(x, y, z);

			vertices.push_back(pos);
		}
	}

	for (unsigned int i = 0; i < stackCount; ++i) {
		unsigned int k1 = i * (sectorCount + 1);
		unsigned int k2 = k1 + sectorCount + 1;

		for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
			if (i != (stackCount - 1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
}

static void generateSphereMesh(std::vector<LightVertex>& vertices, std::vector<GLuint>& indices,
	float radius = 1.0f, unsigned int sectorCount = 36, unsigned int stackCount = 18) {
	const float PI = 3.14159265359f;

	vertices.clear();
	indices.clear();

	for (unsigned int i = 0; i <= stackCount; ++i) {
		float stackAngle = PI / 2.0f - i * PI / stackCount;  // from pi/2 to -pi/2
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		for (unsigned int j = 0; j <= sectorCount; ++j) {
			float sectorAngle = j * 2.0f * PI / sectorCount;

			float x = xy * cosf(sectorAngle);
			float y = xy * sinf(sectorAngle);
			glm::vec3 pos(x, y, z);

			glm::vec3 normal = glm::normalize(pos);
			vertices.push_back({ pos, normal });
		}
	}

	for (unsigned int i = 0; i < stackCount; ++i) {
		unsigned int k1 = i * (sectorCount + 1);
		unsigned int k2 = k1 + sectorCount + 1;

		for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
			if (i != (stackCount - 1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
}

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
	GLSLProgram() : _programID(0), _vertexShaderID(0), _geometryShaderID(0), _fragmentShaderID(0), _numAttributes(0)
	{

	}

	~GLSLProgram() {

	}

	// Vertex + Fragment
	void compileAndLinkShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath) {
		std::string vertSource = readShaderFile(vertexShaderFilePath);
		std::string fragSource = readShaderFile(fragmentShaderFilePath);

		compileAndLinkShadersFromSource(vertSource.c_str(), fragSource.c_str());
	}

	// Vertex + Geometry + Fragment
	void compileAndLinkShaders(const std::string& vertexShaderFilePath, const std::string& geometryShaderFilePath, const std::string& fragmentShaderFilePath) {
		std::string vertSource = readShaderFile(vertexShaderFilePath);
		std::string geomSource = readShaderFile(geometryShaderFilePath);
		std::string fragSource = readShaderFile(fragmentShaderFilePath);

		compileAndLinkShadersFromSource(vertSource.c_str(), geomSource.c_str(), fragSource.c_str());
	}

	void compileAndLinkShadersFromSource(const char* vertexSource, const char* fragmentSource) {

		std::vector<GLuint> shaderIDs;

		shaderIDs.push_back(createAndCompileShader(vertexSource, GL_VERTEX_SHADER, "Vertex"));
		shaderIDs.push_back(createAndCompileShader(fragmentSource, GL_FRAGMENT_SHADER, "Fragment"));

		linkShadersInternal(shaderIDs);
	}

	void compileAndLinkShadersFromSource(const char* vertexSource, const char* geometrySource, const char* fragmentSource) {

		std::vector<GLuint> shaderIDs;

		shaderIDs.push_back(createAndCompileShader(vertexSource, GL_VERTEX_SHADER, "Vertex"));
		shaderIDs.push_back(createAndCompileShader(geometrySource, GL_GEOMETRY_SHADER, "Geometry"));
		shaderIDs.push_back(createAndCompileShader(fragmentSource, GL_FRAGMENT_SHADER, "Fragment"));

		linkShadersInternal(shaderIDs);
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
	GLuint _geometryShaderID;
	GLuint _fragmentShaderID;

	int _numAttributes;

	std::string readShaderFile(const std::string& filePath) {
		std::vector<unsigned char> buffer;
		TextureManager::readFileToBuffer(filePath.c_str(), buffer);
		return std::string(buffer.begin(), buffer.end());
	}

	GLuint createAndCompileShader(const std::string& source, GLenum shaderType, const std::string& typeName) {
		GLuint shaderID = glCreateShader(shaderType);
		if (shaderID == 0) {
			TazGraphEngine::ConsoleLogger::error(typeName + " Shader Failed to create!");
			return 0;
		}

		compileShader(source.c_str(), typeName, shaderID);
		return shaderID;
	}

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

	void linkShadersInternal(const std::vector<GLuint>& shaderIDs) {
		_programID = glCreateProgram();

		// Attach all shaders
		for (GLuint shaderID : shaderIDs) {
			glAttachShader(_programID, shaderID);
		}

		// Link program
		glLinkProgram(_programID);

		// Check link status
		GLint isLinked = 0;
		glGetProgramiv(_programID, GL_LINK_STATUS, &isLinked);

		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(_programID, maxLength, &maxLength, &errorLog[0]);

			// Cleanup on failure
			glDeleteProgram(_programID);
			for (GLuint shaderID : shaderIDs) {
				glDeleteShader(shaderID);
			}

			std::printf("%s\n", &errorLog[0]);
			TazGraphEngine::ConsoleLogger::error("Shaders failed to link");
			return;
		}

		// Cleanup shaders after successful link
		for (GLuint shaderID : shaderIDs) {
			glDetachShader(_programID, shaderID);
			glDeleteShader(shaderID);
		}
	}

};