#pragma once

#include <map>

#include <vector>
#include "../GLTexture.h"
#include <string>

class TextureManager {

public:
	static TextureManager& getInstance();
	//OPENGL functions
	static bool readFileToBuffer(const char* filePath, std::vector <unsigned char>& buffer);
	static GLTexture* loadPNG(const char* filePath);
	//texture management
	void Add_GLTexture(std::string id, const char* path);
	const GLTexture* Get_GLTexture(std::string id);
	std::vector<std::string> Get_GLTextureNames() const;

private:
	std::map<std::string, const GLTexture*> gl_textures;
};