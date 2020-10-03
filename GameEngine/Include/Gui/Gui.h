#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include "System/Updateable.h"

namespace SekhmetEngine
{
	class Gui : public Updateable
	{
	private:
		GLFWwindow* window;
	public:
		void Initialize();
		void OnDestroy();
		bool IsExited();
		void OnUpdate();
		GLFWwindow* GetWindow();
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void ErrorCallback(int error, const char* description);
	};
}