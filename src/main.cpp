#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <nlohmann/json.hpp>

#include "engine/Engine.h"
#include "engine/graphics/Window.h"
#include "world/World.h"
#include "engine/ShaderProgram.h"
//#include "engine/graphics/Window.h"
#include "Refrences.h"
#include "Texture.h"

#include <iostream>
#include <cassert>



void initGlobals();
void mouseFollow(GLFWwindow* window, double xposIn, double yposIn);
void processInput(Camera* camerfa);

// send txc data
// its not being sent rn

//void GLAPIENTRY
//MessageCallback(GLenum source,
//    GLenum type,
//    GLuint id,
//    GLenum severity,
//    GLsizei length,
//    const GLchar* message,
//    const void* userParam)
//{
//    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
//        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
//        type, severity, message);
//}
void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
		}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
		}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
		}();
	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}


int main()
{

    initGlobals();

	engine::init();
	world::initWorld();

	engine::graphics::setMouseCallback(mouseFollow);
	engine::graphics::setMouse(engine::graphics::mouse_mode::DISABLED);
    //Engine engine;
	
	//int d;
	//glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &d);
	//std::cout << "opengl version: " << d << '\n';


    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, 0);


    //engine.setCullFace(1);
    //engine.setDepthTest(true);
    //engine.setOpenglDebugOutput(true);

    Texture stone;
    stone.loadTexture("/Voxels/res/textures/grass_block_top_diffuse.png");
    stone.bind(0);

	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	world::bakeWorld();

	
	ShaderProgram voxelProgram("/Voxels/res/shaders/Block.vert", "/Voxels/res/shaders/Block.frag");

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 20000.0f);
	voxelProgram.setValue("projection", projection);

	

	while (!glfwWindowShouldClose(engine::graphics::getContext()))
	{
		// input
		glfwPollEvents();
		processInput(g_camera.get());

		// update gamestate
		glm::mat4 view = g_camera->getViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		voxelProgram.setValue("view", view);
		voxelProgram.setValue("model", model);


		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		world::renderWorld(voxelProgram);

		// update
		engine::updateFrame();
	}


	voxelProgram.cleanup();
}

void initGlobals()
{
    g_camera = std::make_unique<Camera>(Camera(glm::vec3(0.0f, 41.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void processInput(Camera* camerfa)
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


void mouseFollow(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (g_camera->firstMouse)
	{
		g_camera->lastX = xpos;
		g_camera->lastY = ypos;
		g_camera->firstMouse = false;
	}

	float xoffset = xpos - g_camera->lastX;
	float yoffset = g_camera->lastY - ypos; // reversed since y-coordinates go from bottom to top
	g_camera->lastX = xpos;
	g_camera->lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	g_camera->yaw += xoffset;
	g_camera->pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (g_camera->pitch > 89.0f)
		g_camera->pitch = 89.0f;
	if (g_camera->pitch < -89.0f)
		g_camera->pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(g_camera->yaw)) * cos(glm::radians(g_camera->pitch));
	front.y = sin(glm::radians(g_camera->pitch));
	front.z = sin(glm::radians(g_camera->yaw)) * cos(glm::radians(g_camera->pitch));
	g_camera->orientation = glm::normalize(front);
}