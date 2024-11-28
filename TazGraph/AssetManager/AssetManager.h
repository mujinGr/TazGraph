#pragma once

#include <map>

#include <vector>
#include "../GLTexture.h"
#include <string>

class AssetManager
{
public:
	static bool readFileToBuffer(const char* filePath, std::vector <unsigned char>& buffer);

private:
	std::map<std::string, const GLTexture*> gl_textures;
};

