#pragma once

#include <GLFW/glfw3.h>

#include <string>

namespace engine
{
	namespace graphics
	{
		enum class mouse_mode
		{
			NORMAL,
			HIDDEN,
			DISABLED
		};

		bool initWindow(std::string title, int width, int height);
		void terminate();

		void setMouseCallback(GLFWcursorposfun callback);

		GLFWwindow* getContext();

		void setScrTitle(std::string title);
		void setScrWidth(int width);
		void setScrHeight(int height);

		void setMouse(mouse_mode mode);
	}
}


