/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <iostream>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <enkiTS/TaskScheduler.h>

namespace SekhmetEngine
{
	class Graphics : public enki::ITaskSet
	{
		private:
			int width;
			int height;
		public:
			void Initialize(GLFWwindow* renderTargetWindow);
			void Render();
			void Update();
			void Destroy();
			void SetViewRectSize(int widthIn, int heightIn);
			void ExecuteRange(enki::TaskSetPartition range_, uint32_t threadnum_) override;
	};
}