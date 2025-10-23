#include "Window.h"

namespace engine
{
	namespace graphics
	{
		struct graphics_internal
		{
			std::string scrTitle;
			GLFWwindow* context;
			int scrWidth, scrHeight;
		};

		static graphics_internal _window;

		bool initGlfw();






		bool initWindow(std::string title, int width, int height)
		{
			_window.scrTitle = title;
			_window.scrWidth = width;
			_window.scrHeight = height;
			
			if (!initGlfw())
			{
				std::cout << "ERROR - failed to init glfw\n";
				return false;
			}

			_window.context = glfwCreateWindow(_window.scrWidth, _window.scrHeight, _window.scrTitle.c_str(), nullptr, nullptr);
			if (!_window.context)
			{
				std::cout << "ERROR - failed to create glfw window\n";
				return false;
			}

			glfwMakeContextCurrent(_window.context);


			return true;
		}

		void terminate()
		{
			glfwTerminate();
		}

		void setMouseCallback(GLFWcursorposfun callback)
		{
			glfwSetCursorPosCallback(_window.context, callback);
		}

		GLFWwindow* getContext()
		{
			return _window.context;
		}

		void setScrTitle(std::string title)
		{
			_window.scrTitle = title;

			glfwSetWindowTitle(_window.context, _window.scrTitle.c_str());
		}

		void setScrWidth(int width)
		{
			_window.scrWidth = width;

			glfwSetWindowSize(_window.context, _window.scrWidth, _window.scrHeight);
		}

		void setScrHeight(int height)
		{
			_window.scrHeight = height;

			glfwSetWindowSize(_window.context, _window.scrWidth, _window.scrHeight);
		}

		void setMouse(mouse_mode mode)
		{
			switch (mode)
			{
			case mouse_mode::NORMAL:
				glfwSetInputMode(_window.context, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				break;
			case mouse_mode::HIDDEN:
				glfwSetInputMode(_window.context, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				break;
			case mouse_mode::DISABLED:
				glfwSetInputMode(_window.context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				break;
			}
		}


		bool initGlfw()
		{
			//int initVal = ;
			if (!glfwInit())
				return false;

			glfwWindowHint(GLFW_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_VERSION_MINOR, 4);
			glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
			//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			return true;
		}
	}
}



