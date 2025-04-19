#pragma once

#include "AirBlock.h"
#include "ColoredBlock.h"
#include "StoneBlock.h"

#include <unordered_map>

#define voxel_data std::unordered_map<BlockID, BlockData>

static voxel_data getVoxelDataDictionary()
{
	voxel_data f = {
		{ DEFAULT_AIR_BLOCK, getAirBlockData() },
		{ COLORED_BLOCK, getColoredBlockData() },
		{ STONE_BLOCK, getStoneBlockData() }
	};
	return f;
}
