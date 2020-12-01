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

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		VkInstanceCreateInfo vulkanInstanceCreateInfo{};
		vulkanInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		vulkanInstanceCreateInfo.pApplicationInfo = &vulkanAppInfo;
		vulkanInstanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
		vulkanInstanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
		vulkanInstanceCreateInfo.enabledLayerCount = 0;

		if (vkCreateInstance(&vulkanInstanceCreateInfo, nullptr, &vulkanInstance) != VK_SUCCESS)
		{
			throw std::exception("Failed to create vulkan instance");
		}

		//load shaders

		entities = entitiesIn;
	}

	void Graphics::Update()
	{
	}

	void Graphics::Destroy()
	{
		vkDestroyInstance(vulkanInstance, nullptr);
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
}