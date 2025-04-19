#pragma once

#include "engine/graphics/Camera.h"
#include "world/voxels/VoxelDataDictionary.h"

#include <memory>

extern std::shared_ptr<Camera> g_camera;
extern std::unique_ptr<voxel_data> g_VoxelData;