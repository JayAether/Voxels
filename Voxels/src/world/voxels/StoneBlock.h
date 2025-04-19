#pragma once

#include "Block.h"
#include "BlockID.h"

static BlockData getStoneBlockData()
{
	return BlockData(
		STONE_BLOCK, 
		"resources/shaders/block.vert", 
		"resources/shaders/block.frag", 
		"Stone Shader", 
		"resources/textures/stone.jpg");
}
