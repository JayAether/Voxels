#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "graphics/Camera.h"
#include "graphics/Window.h"
#include "../world/World.h"

#include <string>
#include <memory>

//namespace mouse_callback_N
//{
//	bool firstMouse = true;
//	//float lastX = 400, lastY = 300;
//	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
//	float pitch = 0.0f;
//	float lastX = 640 / 2;
//	float lastY = 480 / 2;
//
//	Camera* camera;
//
//	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//	{
//		float xpos = static_cast<float>(xposIn);
//		float ypos = static_cast<float>(yposIn);
//
//		if (firstMouse)
//		{
//			lastX = xpos;
//			lastY = ypos;
//			firstMouse = false;
//		}
//
//		float xoffset = xpos - lastX;
//		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//		lastX = xpos;
//		lastY = ypos;
//
//		float sensitivity = 0.1f; // change this value to your liking
//		xoffset *= sensitivity;
//		yoffset *= sensitivity;
//
//		yaw += xoffset;
//		pitch += yoffset;
//
//		// make sure that when pitch is out of bounds, screen doesn't get flipped
//		if (pitch > 89.0f)
//			pitch = 89.0f;
//		if (pitch < -89.0f)
//			pitch = -89.0f;
//
//		glm::vec3 front;
//		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//		front.y = sin(glm::radians(pitch));
//		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//		camera->orientation = glm::normalize(front);
//	}
//}


class Engine
{

public:
	Engine();
	~Engine();

	//void initWindow();
	//void initWorld();
	//void initPlayer();


	void mainLoop();

	void updateBackgroundColor(glm::vec3 rgb);
	void updateBackgroundColor(float r, float g, float b);

	void setDepthTest(bool mode);
	void setOpenglDebugOutput(bool mode);
	void setCullFace(int mode);

private:

	void createWindow();

	void processInput(Camera* camera);
	bool gameShouldClose(); // dont like this name/thing, need to find a better thingy

	void calculateDeltaTime();

	void mouseTracingMode(bool mode);


	void renderBackgroundColor(); // ie the sky


public:
	float m_deltaTime;
	
private:
	std::shared_ptr<Window> m_window;
	std::shared_ptr<World> m_world;
	

	//Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	float m_lastTime = 0.0f;
	float m_currentTime = 0.0f;



	bool m_firstInit = true;

	static const int OPENGL_MAJOR_VER = 3;
	static const int OPENGL_MINOR_VER = 3;

	glm::vec3 m_backgroundColor = glm::vec3(0.2f, 0.3f, 0.3f);

	bool m_opengl_depthTest = true;
	bool m_opengl_debugOutput = true;
	bool m_opengl_cullFace = false;
};

