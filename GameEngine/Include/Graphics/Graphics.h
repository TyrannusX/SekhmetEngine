/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <enkiTS/TaskScheduler.h>
#include <bgfx/bgfx.h>
#include "Components/StaticMeshComponent.h"
#include "Entity/Entity.h"

namespace SekhmetEngine
{
	class Graphics : public enki::ITaskSet
	{
		private:
			int width;
			int height;
			std::vector<Entity*> entities;
			bgfx::ShaderHandle vertexShader;
			bgfx::ShaderHandle fragmentShader;
			bgfx::ProgramHandle programHandle;
		public:
			void Initialize(GLFWwindow* renderTargetWindow, std::vector<Entity*> entitiesIn);
			void Render();
			void Update();
			void Destroy();
			void SetViewRectSize(int widthIn, int heightIn);
			bgfx::ShaderHandle LoadShader(std::string shaderPath);
			void ExecuteRange(enki::TaskSetPartition range_, uint32_t threadnum_) override;
	};
}