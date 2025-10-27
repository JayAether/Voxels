#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>


#include "engine/Engine.h"
#include "engine/graphics/Window.h"
#include "world/World.h"
#include "engine/ShaderProgram.h"
//#include "engine/graphics/Window.h"
#include "Refrences.h"
#include "Texture.h"

#include <iostream>
#include <cassert>


//#define tyon_error( message )                                       \
//        tyon::log_error_format( "Tachyon Error", "{} @ {}:{}: {}",               \
//                          __FUNCTION__, __FILE__, __LINE__, message );  \
//        log_flush();
//
//// TYON_BREAK should be valid in release builds
//#define TYON_SIGTRAP 5
//
//#if (REFLECTION_COMPILER_CLANG)
//#define FORCEINLINE __attribute__((always_inline))
//#define TYON_FORCEINLINE __attribute__((always_inline))
//#define TYON_BREAK() __builtin_debugtrap();
//#define TYON_PREFETCH_CACHELINE( address ) __builtin_prefetch( address );
//#elif (REFLECTION_COMPILER_GCC)
//#define FORCEINLINE __attribute__((always_inline))
//#define TYON_FORCEINLINE __attribute__((always_inline))
//#define TYON_BREAK() raise(TYON_SIGTRAP);
//#define TYON_PREFETCH_CACHELINE( address ) __builtin_prefetch( address );
//
//#elif (REFLECTION_COMPILER_MSVC)
//#define FORCEINLINE __forceinline
//#define TYON_FORCEINLINE __forceinline
//#define TYON_BREAK() __debugbreak();
//#define TYON_PREFETCH_CACHELINE( address ) PrefetchCacheLine( PF_TEMPORAL_LEVEL_1, (address) );
//#else
//#define FORCEINLINE
//#define TYON_FORCEINLINE
//#define TYON_BREAK() raise(TYON_SIGTRAP);
//#define TYON_PREFETCH_CACHELINE( address ) ERROR_PREFETCH_NOT_DEFINED
//#endif // compiler
//
//#define ERROR_GUARD( condition, message )                           \
//        if ( !(condition) )                                             \
//        {                                                               \
//            tyon_errorf( "Error Guard", "Condition: ({}): {}",          \
//                         #condition, message );                         \
//            TYON_BREAK();                                               \
//        };



void initGlobals();
void mouseFollow(GLFWwindow* window, double xposIn, double yposIn);
void processInput(Camera* camerfa);

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

	engine::init();
	world::initWorld();

	engine::graphics::setMouseCallback(mouseFollow);
	engine::graphics::setMouse(engine::graphics::mouse_mode::DISABLED);
    //Engine engine;


    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    //engine.setCullFace(1);
    //engine.setDepthTest(true);
    //engine.setOpenglDebugOutput(true);

    Texture stone;
    stone.loadTexture("D:/Voxels/res/textures/Voxels simple grass.png");
    stone.bind(0);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	world::bakeWorld();

	
	ShaderProgram voxelProgram("D:\\Voxels\\res\\shaders\\Block.vert", "D:\\Voxels\\res\\shaders\\Block.frag");

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