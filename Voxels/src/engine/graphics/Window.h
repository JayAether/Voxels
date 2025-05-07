#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Camera.h"

#include <string>
#include <iostream>
#include <memory>




class Window
{
public:
	Window(std::string windowTitle = "nut bags");

	int createWindow();

	//void mouseFollow_callback(GLFWwindow* window, double xposIn, double yposIn)
	//{
	//	float xpos = static_cast<float>(xposIn);
	//	float ypos = static_cast<float>(yposIn);

	//	if (firstMouse)
	//	{
	//		lastX = xpos;
	//		lastY = ypos;
	//		firstMouse = false;
	//	}

	//	float xoffset = xpos - lastX;
	//	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	//	lastX = xpos;
	//	lastY = ypos;

	//	float sensitivity = 0.1f; // change this value to your liking
	//	xoffset *= sensitivity;
	//	yoffset *= sensitivity;

	//	yaw += xoffset;
	//	pitch += yoffset;

	//	// make sure that when pitch is out of bounds, screen doesn't get flipped
	//	if (pitch > 89.0f)
	//		pitch = 89.0f;
	//	if (pitch < -89.0f)
	//		pitch = -89.0f;

	//	glm::vec3 front;
	//	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	//	front.y = sin(glm::radians(pitch));
	//	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	//	//camera->orientation = glm::normalize(front);
	//}

	void updateWindowSize(int width, int height);

	void updateWindowTitle(std::string name);

	void updateWindowTitle(const char* name);

	void mouseTracingMode(bool mode);
	
	float getScrWidth()
	{
		return m_scrWidth;
	}

	float getScrHeight()
	{
		return m_scrHeight;
	}

	GLFWwindow* getWindow() const
	{
		return m_window;
	}

	

public:
	//GLFWwindow* window;
	GLFWwindow* m_window;
	

	//bool firstMouse = true;
	////float lastX = 400, lastY = 300;
	//float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	//float pitch = 0.0f;
	//float lastX = 640 / 2;
	//float lastY = 480 / 2;

	std::string m_windowTitle;
	int m_scrWidth;
	int m_scrHeight;





	//bool firstMouse = true;
	////float lastX = 400, lastY = 300;
	//float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	//float pitch = 0.0f;
	//float lastX = 640 / 2;
	//float lastY = 480 / 2;
};

