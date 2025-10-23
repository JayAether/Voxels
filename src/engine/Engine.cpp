#include "Engine.h"

#include "glad/glad.h"


#include "graphics/Window.h"
#include "world/World.h"
//#include "../Refrences.h"

#include <string>
#include <iostream>
#include <exception>




namespace engine
{

    
    //ff::

    struct engine_internal
    {
        double currentFrameTime = 0.0;
        double lastFrameTime = 0.0;
    };

    inline engine_internal i_engine;

    bool init()
    {

        graphics::initWindow("Tit le", 800, 600);

        if (!gladLoadGL())
        {
            std::cout << "ERROR - failed to load glad\n";
            return false;
        }

        world::initWorld();

    }
    
    void terminate()
    {
        graphics::terminate();
    }

    void calculateDeltaTime()
    {
        i_engine.currentFrameTime = glfwGetTime();
        g_engine.deltaTime = i_engine.currentFrameTime - i_engine.lastFrameTime;
        i_engine.lastFrameTime = i_engine.currentFrameTime;
    }

    void updateFrame()
    {
        calculateDeltaTime();

        glfwSwapBuffers(graphics::getContext());
    }

    void enableDebugMode()
    {
        glEnable(GL_DEBUG_OUTPUT);
    }
}








//void Engine::mainLoop()
//{
//    m_world = std::make_shared<World>();
//
//    m_world->generateWorld();
//    m_world->bakeWorld();
//    
//    // deoedd: and then kick off the next frame by handling input, update gamestate, start render, handle network, wait for render, wait for min frame - time, swap framebuffers and repeat
//
//    
//    ShaderProgram voxelProgram("D:\\Voxels\\res\\shaders\\Block.vert", "D:\\Voxels\\res\\shaders\\Block.frag");
//
//
//    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)m_window->getScrWidth() / (float)m_window->getScrHeight(), 0.1f, 20000.0f);
//    voxelProgram.setValue("projection", projection);
//    
//    while (!gameShouldClose())
//    {
//        processInput(g_camera.get());
//        glfwPollEvents();
//
//        calculateDeltaTime();
//
//        renderBackgroundColor();z
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        glm::mat4 view = g_camera->getViewMatrix();
//        glm::mat4 model = glm::mat4(1.0f);
//
//        voxelProgram.setValue("view", view);
//        voxelProgram.setValue("model", model);
//        
//        m_world->render(voxelProgram);
//
//
//        glfwSwapBuffers(m_window->getWindow());
//
//        m_window->updateWindowTitle(std::to_string(1.0f / m_deltaTime));
//
//        //GLenum g = glGetError();
//        //printf("%i", g);
//    }
//}
//
//void Engine::mouseTracingMode(bool mode)
//{
//    m_window->mouseTracingMode(mode);
//
//    if (mode)
//    {
//        //m_window->mouseTracingMode(mode);
//        ////glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//        //glfwSetCursorPosCallback(window, mouse_callback_N::mouse_callback);
//        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//    }
//}

