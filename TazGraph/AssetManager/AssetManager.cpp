#include "AssetManager.h"

#include <fstream>
#include "../ConsoleLogger/ConsoleLogger.h"

bool AssetManager::readFileToBuffer(const char* filePath, std::vector<unsigned char>& buffer)
{
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
