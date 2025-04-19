#pragma once

#include "Block.h"
#include "BlockID.h"

static BlockData getAirBlockData()
{
	return BlockData(
		DEFAULT_AIR_BLOCK,
		"resources/shaders/block.vert",
		"resources/shaders/block.frag",
		"Air Shader",
		"resources/textures/air.jpg");
}
