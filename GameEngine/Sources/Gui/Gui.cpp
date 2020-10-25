/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include "Gui/Gui.h"

namespace SekhmetEngine
{
	void Gui::Initialize()
	{
		std::cout << "Initialize GUI" << std::endl;

		if (!glfwInit())
		{
			throw std::exception("Failed to initialize GLFW");
		}

		glfwSetErrorCallback(ErrorCallback);

		window = glfwCreateWindow(1440, 900, "Sekhmet Engine", NULL, NULL);
		if (!window)
		{
			throw std::exception("Failed to create window with rendering context");
		}

		glfwSetKeyCallback(window, KeyCallback);
	}

	void Gui::Destroy()
	{
		std::cout << "Destroying GUI" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Gui::ErrorCallback(int error, const char* description)
	{
		fprintf(stderr, "GLFW Error: %s\n", description);
	}

	void Gui::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	bool Gui::IsExited()
	{
		return glfwWindowShouldClose(window);
	}

	void Gui::Update()
	{
		glfwPollEvents();
	}

	GLFWwindow* Gui::GetWindow()
	{
		return window;
	}
}