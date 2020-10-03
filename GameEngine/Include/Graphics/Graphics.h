#pragma once
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <iostream>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "System/Updateable.h"

namespace SekhmetEngine
{
	class Graphics : public Updateable
	{
		private:
			int width;
			int height;
		public:
			void Initialize(GLFWwindow* renderTargetWindow);
			void OnUpdate();
			void OnDestroy();
	};
}