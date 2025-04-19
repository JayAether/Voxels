#include "engine/Engine.h"
#include "Refrences.h"

void initGlobals();


int main()
{
    initGlobals();

    Engine engine;

    engine.setCullFace(true, 0);
    engine.setDepthTest(true);

    engine.mainLoop();
}

void initGlobals()
{
    g_camera = std::make_unique<Camera>(Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    g_VoxelData = std::make_unique<voxel_data>();
}
