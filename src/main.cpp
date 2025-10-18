#include "engine/Engine.h"
#include "Refrences.h"

#include "Texture.h"

void initGlobals();

// send txc data
// its not being sent rn

void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}

int main()
{
    initGlobals();

    Engine engine;


    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    engine.setCullFace(1);
    engine.setDepthTest(true);
    engine.setOpenglDebugOutput(true);


    Texture stone;
    stone.loadTexture("D:/Voxels/res/textures/container.png");
    stone.bind(0);


    engine.mainLoop();
}

void initGlobals()
{
    g_camera = std::make_unique<Camera>(Camera(glm::vec3(0.0f, 41.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
}
