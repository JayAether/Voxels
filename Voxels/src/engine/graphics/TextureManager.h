#pragma once

#include "Texture.h"
#include "../../world/voxels/BlockID.h"

#include <unordered_map>
#include <list>

class TextureManager
{
private:
	struct TextureBlockPaths
	{
		std::list<std::string> texturePaths;
	};
public:
	TextureManager();
	~TextureManager();

	void createTexture(BlockID id);

	void getTexture(BlockID id);

private:
	TextureBlockPaths parseBlockResourceData(BlockID id);

private:
	std::unordered_multimap<BlockID, Texture> m_textures;
	std::unordered_multimap<BlockID, std::string> m_textureStrings;
};

