#include "TextureManager.h"

#include "../../world/voxels/BlockDataKeyword.h"

#include <fstream>

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{

}


void TextureManager::createTexture(BlockID id)
{
	//m_textures.push_back(Texture(path));
	TextureBlockPaths paths = parseBlockResourceData(id);
}


void TextureManager::getTexture(BlockID id)
{

}

TextureManager::TextureBlockPaths TextureManager::parseBlockResourceData(BlockID id)
{
	TextureBlockPaths texs;

	std::ifstream stream;

	std::string line;
	// TODO: find a way to link IDs to the blockdata


	while (std::getline(stream, line))
	{
		if (line.find(TEXTURE_PATH_DATA_KEY))
		{
			texs.texturePaths.push_back(line.substr(line.find("=") + 1));
		}
	}

	return texs;
}

