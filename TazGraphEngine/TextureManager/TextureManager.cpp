#include "TextureManager.h"
#include <fstream>
#include "../picoPNG.h"

#include "../ConsoleLogger.h"


TextureManager& TextureManager::getInstance() {
	static TextureManager instance;
	return instance;
}

bool TextureManager::readFileToBuffer(const char* filePath, std::vector <unsigned char>& buffer) {
	std::ifstream file(filePath, std::ios::binary);

	if (file.fail()) {
		perror(filePath);
		return false;
	}

	//seek to the end
	file.seekg(0, std::ios::end);

	//Get the file size
	int fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	//Reduce the file size by any header bytes that might be present
	fileSize -= file.tellg();

	buffer.resize(fileSize);
	file.read((char*)&(buffer[0]), fileSize);
	file.close();

	return true;
}

GLTexture* TextureManager::loadPNG(const char* filePath) {
	GLTexture* texture = new GLTexture{};

	std::vector<unsigned char> in;
	std::vector<unsigned char> out;

	unsigned long width, height;
	if (TextureManager::readFileToBuffer(filePath, in) == false) {
		TazGraphEngine::ConsoleLogger::error("Failed to load PNG file to buffer!");
	}

	int errorCode = decodePNG(out, width, height, &(in[0]), in.size(), true);

	if (errorCode != 0) {
		TazGraphEngine::ConsoleLogger::error("decodePNG failed with error: " + std::to_string(errorCode));
	}

	glGenTextures(1, &(texture->id));

	glBindTexture(GL_TEXTURE_2D, texture->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(out[0]));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	texture->width = width;
	texture->height = height;

	return texture;
}


void TextureManager::Add_GLTexture(std::string id, const char* path)
{
	const GLTexture* testPNG = TextureManager::loadPNG(path);
	const int testHeight = testPNG->height;
	const int testWidth = testPNG->width;
	const unsigned int testId = testPNG->id;
	const GLTexture* test1PNG = new GLTexture{ testId, testWidth, testHeight };

	std::cout << id << "     " << test1PNG->width << std::endl;
	std::cout << id << "     " << testPNG->width << std::endl;
	std::cout << testHeight << std::endl;
	gl_textures.emplace(id, test1PNG);
	std::cout << id << "     " << (*gl_textures[id]).height << std::endl;
}

const GLTexture* TextureManager::Get_GLTexture(std::string id)
{
	return gl_textures[id];
}
