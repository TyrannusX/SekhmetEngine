/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include <fstream>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Graphics/Graphics.h"
#include "Graphics/Vertex.h"

namespace SekhmetEngine
{
	void Graphics::Initialize(GLFWwindow* renderTargetWindow, std::vector<Entity*> entitiesIn)
	{
		//Get the current window size
		glfwGetWindowSize(renderTargetWindow, &width, &height);

		//initialize vulkan instance/renderer
		VkApplicationInfo vulkanAppInfo{};
		vulkanAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		vulkanAppInfo.pApplicationName = "Sekhmet Engine";
		vulkanAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		vulkanAppInfo.pEngineName = "Sekhmet Engine";
		vulkanAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		vulkanAppInfo.apiVersion = VK_API_VERSION_1_0;

		//Get instance extension details from Vulkan API
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> instanceExtensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());

		//Get instance layer details from Vulkan API for simple/built in validation
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> instanceLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, instanceLayers.data());

		//Pull required Vulkan Instance extensions from GLFW
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		//Ensure Vulkan supports GLFW extensions (wont have a window otherwise)
		std::cout << "CHECK FOR REQUIRED VULKAN INSTANCE EXTENSIONS" << std::endl;
		for (int i = 0; i < glfwExtensionCount; i++)
		{
			bool error = true;
			for (const auto& entry : instanceExtensions)
			{
				if ((strcmp(entry.extensionName, glfwExtensions[i]) != 0) && error)
				{
					error = true;
				}
				else
				{
					error = false;
				}
			}

			if (error)
			{
				throw std::exception("Vulkan Instance Extensions Do Not Support GLFW");
			}
		}

		std::vector<const char*> extensionsVector(glfwExtensions, glfwExtensions + glfwExtensionCount);
		extensionsVector.push_back("VK_EXT_debug_utils");

		//Check for built in validation layer
		std::cout << "CHECK FOR REQUIRED VULKAN INSTANCE LAYERS" << std::endl;
		bool builtinValidationLayerFound = false;
		for (const auto& entry : instanceLayers)
		{
			if (strcmp(entry.layerName, "VK_LAYER_KHRONOS_validation") == 0)
			{
				builtinValidationLayerFound = true;
			}
		}

		if (!builtinValidationLayerFound)
		{
			throw std::exception("Builtin Validation Layer Not Found");
		}

		//Setup debug message callback
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
		debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugMessengerCreateInfo.pfnUserCallback = debugCallback;
		debugMessengerCreateInfo.pUserData = nullptr;

		//Create Vulkan Instance
		const std::vector<const char*> layers = {
			"VK_LAYER_KHRONOS_validation"
		};
		VkInstanceCreateInfo vulkanInstanceCreateInfo{};
		vulkanInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		vulkanInstanceCreateInfo.pApplicationInfo = &vulkanAppInfo;
		vulkanInstanceCreateInfo.enabledExtensionCount = (uint32_t)extensionsVector.size();
		vulkanInstanceCreateInfo.ppEnabledExtensionNames = extensionsVector.data();
		vulkanInstanceCreateInfo.ppEnabledLayerNames = layers.data();
		vulkanInstanceCreateInfo.enabledLayerCount = 0;
		vulkanInstanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;

		std::cout << "CREATING VULKAN INSTANCE" << std::endl;
		if (vkCreateInstance(&vulkanInstanceCreateInfo, nullptr, &vulkanInstance) != VK_SUCCESS)
		{
			throw std::exception("Failed to create vulkan instance");
		}

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
		func(vulkanInstance, &debugMessengerCreateInfo, nullptr, &debugMessenger);

		//load shaders

		entities = entitiesIn;
	}

	void Graphics::Update()
	{
	}

	void Graphics::Destroy()
	{
		//destroy the debug callback
		/*auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
		func(vulkanInstance, debugMessenger, nullptr);
		vkDestroyInstance(vulkanInstance, nullptr);*/
	}

	void Graphics::ExecuteRange(enki::TaskSetPartition range_, uint32_t threadnum_)
	{
		Render();
		Update();
	}

	void Graphics::SetViewRectSize(int widthIn, int heightIn)
	{
		width = widthIn;
		height = heightIn;
	}

	void Graphics::Render()
	{
		//Get component's model
		//const aiScene* scene
		StaticMeshComponent* staticMeshComponent = (StaticMeshComponent*)entities[0]->GetComponents()[0];
		std::vector<tinyobj::shape_t> shape = staticMeshComponent->GetModel();
		tinyobj::attrib_t attributes = staticMeshComponent->GetAttributes();
		std::vector<float> vertices;

		//loop over all vertex entries in the obj
		for (unsigned int i = 0; i < shape.size(); i++)
		{
			unsigned int index_offset = 0;
			for (unsigned int j = 0; j < shape[i].mesh.num_face_vertices.size(); j++)
			{
				//current row in obj
				unsigned int faceVertices = shape[i].mesh.num_face_vertices[j];
				for (unsigned int k = 0; k < faceVertices; k++)
				{
					tinyobj::index_t index = shape[i].mesh.indices[index_offset + k];

					//get position
					vertices.push_back((float)attributes.vertices[3 * index.vertex_index + 0]);
					vertices.push_back((float)attributes.vertices[3 * index.vertex_index + 1]);
					vertices.push_back((float)attributes.vertices[3 * index.vertex_index + 2]);

					//get normal
					vertices.push_back((float)attributes.normals[3 * index.normal_index + 0]);
					vertices.push_back((float)attributes.normals[3 * index.normal_index + 1]);
					vertices.push_back((float)attributes.normals[3 * index.normal_index + 2]);

					//get texture coordinates
					vertices.push_back((float)attributes.texcoords[2 * index.texcoord_index + 0]);
					vertices.push_back((float)attributes.texcoords[2 * index.texcoord_index + 1]);
				}
				index_offset += faceVertices;
			}
		}

		//Define what the vertex looks like in the vector

		//create vertex buffer
	}

	void Graphics::LoadShader(std::string shaderPath)
	{
		//setup shader file handle with binary format
		std::ifstream shaderFileHandle(shaderPath, std::ios::ate | std::ios::binary);

		//get shader file size by seeking to end and resetting to beginning
		size_t fileSize = (size_t)shaderFileHandle.tellg();
		shaderFileHandle.seekg(0, std::ios::beg);

		//read the binary data
		std::vector<char> shaderBinaryData(fileSize);
		shaderFileHandle.read(shaderBinaryData.data(), fileSize);

		//copy the binary data into a bgfx memory handler

		//create a shader handler

		shaderFileHandle.close();
	}

	//Callback function for Vulkan to pass validation layer error messages back to
	VKAPI_ATTR VkBool32 VKAPI_CALL Graphics::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData
	)
	{
		std::cerr << "validation layer: " << callbackData->pMessage << std::endl;
		return VK_FALSE;
	}
}