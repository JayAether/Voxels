#pragma once

#include <stdint.h>
#include <string>


enum BlockID : uint16_t
{
	DEFAULT_AIR_BLOCK,
	COLORED_BLOCK,
	STONE_BLOCK,
	GRASS_BLOCK,
	DIRT_BLOCK,

	BLOCK_COUNT
};

struct BlockData
{
	BlockID id;
	std::string vertexPath;
	std::string fragmentPath;
	std::string shaderProgram;
	std::string texturePath;

	BlockData(BlockID id, std::string vertexPath, std::string fragmentPath, std::string shaderProgram, std::string texturePath)
	{
		this->id = id;
		this->vertexPath = vertexPath;
		this->fragmentPath = fragmentPath;
		this->shaderProgram = shaderProgram;
		this->texturePath = texturePath;
	}
};