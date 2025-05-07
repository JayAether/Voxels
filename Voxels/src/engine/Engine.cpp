#include "Engine.h"



#include "../Refrences.h"

#include <string>
#include <iostream>
#include <exception>

Engine::Engine()
{
    if (!m_firstInit)
    {
        return;
    }

    if (!glfwInit())
    {
        glfwTerminate();
        throw std::exception("failed to load glfw");;
        return;
    }


    createWindow();
    
    glfwInitHint(GLFW_VERSION_MAJOR, OPENGL_MAJOR_VER);
    glfwInitHint(GLFW_VERSION_MINOR, OPENGL_MINOR_VER);
    glfwInitHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int version = gladLoadGL();
    if (version == 0)
    {
        throw std::exception("failed to load glad");
        return;
    }

    //g_camera = std::make_shared<Camera>(Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));


    m_firstInit = false;
}






Engine::~Engine()
{
    glfwTerminate();
}



void Engine::createWindow()
{
    m_window = std::make_shared<Window>();
}


void Engine::mainLoop()
{
    m_world = std::make_shared<World>();

    m_world->generateWorld();
    m_world->bakeWorld();
    
    // deoedd: and then kick off the next frame by handling input, update gamestate, start render, handle network, wait for render, wait for min frame - time, swap framebuffers and repeat

    
    ShaderProgram voxelProgram("res/shaders/Block.vert", "res/shaders/Block.frag");


    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)m_window->getScrWidth() / (float)m_window->getScrHeight(), 0.1f, 20000.0f);
    voxelProgram.setValue("projection", projection);
    
    while (!gameShouldClose())
    {
        processInput(g_camera.get());
        glfwPollEvents();

        calculateDeltaTime();

        renderBackgroundColor();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = g_camera->getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        voxelProgram.setValue("view", view);
        voxelProgram.setValue("model", model);
        
        m_world->render(voxelProgram);


        glfwSwapBuffers(m_window->getWindow());

        m_window->updateWindowTitle(std::to_string(1.0f / m_deltaTime));

        //GLenum g = glGetError();
        //printf("%i", g);
    }
}

void Engine::mouseTracingMode(bool mode)
{
    m_window->mouseTracingMode(mode);

    if (mode)
    {
        //m_window->mouseTracingMode(mode);
        ////glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        //glfwSetCursorPosCallback(window, mouse_callback_N::mouse_callback);
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}





void Engine::updateBackgroundColor(glm::vec3 rgb)
{
    if (rgb.x > 1.0f)
        rgb.x = 1.0f;
    if (rgb.x < 0.0f)
        rgb.x = 0.0f;
    if (rgb.y > 1.0f)
        rgb.y = 1.0f;
    if (rgb.y < 0.0f)
        rgb.y = 0.0f;
    if (rgb.z > 1.0f)
        rgb.z = 1.0f;
    if (rgb.z < 0.0f)
        rgb.z = 0.0f;

    m_backgroundColor = rgb;
}

void Engine::updateBackgroundColor(float r, float g, float b)
{
    if (r > 1.0f)
        r = 1.0f;
    if (r < 0.0f)
        r = 0.0f;
    if (g > 1.0f)
        g = 1.0f;
    if (g < 0.0f)
        g = 0.0f;
    if (b > 1.0f)
        b = 1.0f;
    if (b < 0.0f)
        b = 0.0f;

    m_backgroundColor = glm::vec3(r, g, b);
}


void Engine::renderBackgroundColor()
{
    glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1.0f);
}


void Engine::processInput(Camera* camerfa)
{
    Camera *camera = g_camera.get();

    float speed = 0.5f;
    GLFWwindow* context = glfwGetCurrentContext();

    if (glfwGetKey(context, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(context, true);
    if (glfwGetKey(context, GLFW_KEY_W) == GLFW_PRESS)
        camera->position += glm::normalize(camera->orientation) * speed;
    if (glfwGetKey(context, GLFW_KEY_S) == GLFW_PRESS)
        camera->position -= glm::normalize(camera->orientation) * speed;
    if (glfwGetKey(context, GLFW_KEY_A) == GLFW_PRESS)
        camera->position -= glm::normalize(glm::cross(camera->orientation, camera->up)) * speed;
    if (glfwGetKey(context, GLFW_KEY_D) == GLFW_PRESS)
        camera->position += glm::normalize(glm::cross(camera->orientation, camera->up)) * speed;
}


bool Engine::gameShouldClose()
{
    return glfwWindowShouldClose(m_window->getWindow());
}

void Engine::calculateDeltaTime()
{
    m_currentTime = (float)glfwGetTime(); // im casting it to float rn, but idk if i want it to be float or double
    m_deltaTime = m_currentTime - m_lastTime;
    m_lastTime = m_currentTime;
}


void Engine::setDepthTest(bool mode)
{
    if (mode)
    {
        m_opengl_depthTest = true;
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        m_opengl_depthTest = false;
        glDisable(GL_DEPTH_TEST);
    }
}

void Engine::setOpenglDebugOutput(bool mode)
{
    if (mode)
    {
        m_opengl_debugOutput = true;
        glEnable(GL_DEBUG_OUTPUT);
    }
    else
    {
        m_opengl_debugOutput = false;
        glDisable(GL_DEBUG_OUTPUT);
    }
}

void Engine::setCullFace(int mode)
{
    switch (mode)
    {
    case 0:
        m_opengl_cullFace = false;
        glDisable(GL_CULL_FACE);
        break;
    case 1:
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);
        break;
    case 2:
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        break;
    case 3:
        glCullFace(GL_FRONT_AND_BACK);
        glEnable(GL_CULL_FACE);
        break;
    default:
        throw std::runtime_error("there is no fourth cull face");
        break;
    }
}


