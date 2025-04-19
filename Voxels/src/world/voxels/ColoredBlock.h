#pragma once

#include "Block.h"
#include "BlockID.h"

static BlockData getColoredBlockData()
{
	return BlockData(
		COLORED_BLOCK,
		"resources/shaders/block.vert",
		"resources/shaders/block.frag",
		"Colored Shader",
		"resources/textures/colored.jpg");
}

