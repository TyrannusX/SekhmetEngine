/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#include <iostream>
#include <GLFW/glfw3.h>

namespace SekhmetEngine
{
	class Gui
	{
	private:
		GLFWwindow* window;
	public:
		void Initialize();
		void Destroy();
		bool IsExited();
		void Update();
		GLFWwindow* GetWindow();
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void ErrorCallback(int error, const char* description);
	};
}