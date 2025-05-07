#pragma once

#include <stdint.h>


enum BlockID : uint16_t
{
	DEFAULT_AIR_BLOCK,
	COLORED_BLOCK,
	STONE_BLOCK,
	GRASS_BLOCK,
	DIRT_BLOCK,

	BLOCK_COUNT
};
