#include "Window.h"

#include "../../Refrences.h"

namespace fff
{
	static void mouseFollow_callback(GLFWwindow* window, double xposIn, double yposIn)
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
};



Window::Window(std::string windowTitle)
	: m_windowTitle(windowTitle), m_scrWidth(640), m_scrHeight(480)
{

	m_window = glfwCreateWindow(m_scrWidth, m_scrHeight, m_windowTitle.c_str(), NULL, NULL);
	if (!m_window)
	{
		throw std::exception("failed to load window");
		return;
	}

	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, this);


	mouseTracingMode(true);

	//glViewport(0, 0, m_scrWidth, m_scrHeight);


}


void Window::updateWindowSize(int width, int height)
{
	glfwSetWindowSize(m_window, width, height);
	//glViewport(0, 0, width, height);

	m_scrWidth = width;
	m_scrHeight = height;
}

void Window::updateWindowTitle(std::string name)
{
	m_windowTitle = name;
	glfwSetWindowTitle(m_window, name.c_str());
}

void Window::updateWindowTitle(const char* name)
{
	m_windowTitle = name;
	glfwSetWindowTitle(m_window, name);
}

void Window::mouseTracingMode(bool mode)
{
	if (mode)
	{
		glfwSetCursorPosCallback(m_window, fff::mouseFollow_callback);
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		glfwSetCursorPosCallback(m_window, NULL);
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
	}
}


float Window::getScrWidth()
{
	return m_scrWidth;
}

float Window::getScrHeight()
{
	return m_scrHeight;
}

GLFWwindow* Window::getWindow() const
{
	return m_window;
}