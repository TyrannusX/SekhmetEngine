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
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "Components/StaticMeshComponent.h"
#include "Entity/Entity.h"

namespace SekhmetEngine
{
	class Graphics : public enki::ITaskSet
	{
		private:
			int width;
			int height;
			std::vector<Entity*> entities; //list of entities to iterate through
			VkInstance vulkanInstance; //instance (container for Vulkan to run in)
			VkPhysicalDevice vulkanPhysicalDevice; //physical devie (selected graphics card)
			VkDevice vulkanLogicalDevice; //logical device that submits commands to queue for physical device to process
			VkQueue vulkanGraphicsQueueHandle; //queue that passes commands from the logical device to the physical device
			VkQueue vulkanPresentationQueueHandle;
			VkDebugUtilsMessengerEXT debugMessenger; //debug messenger
			VkSurfaceKHR vulkanSurface; //surface that Vulkan "draws" to. Interfaces with underlying windows system (GLFW in this case)

			//Debug Callback for Vulkan to Print to stderr
			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
				void* userData
			);
		public:
			void Initialize(GLFWwindow* renderTargetWindow, std::vector<Entity*> entitiesIn);
			void Render();
			void Update();
			void Destroy();
			void SetViewRectSize(int widthIn, int heightIn);
			void LoadShader(std::string shaderPath);
			void ExecuteRange(enki::TaskSetPartition range_, uint32_t threadnum_) override;
	};
}