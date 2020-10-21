/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include <fstream>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <glm/vec3.hpp>
#include "Graphics/Graphics.h"

namespace SekhmetEngine
{
	void Graphics::Initialize(GLFWwindow* renderTargetWindow, std::vector<Entity*> entitiesIn)
	{
		std::cout << "Initialize Graphics" << std::endl;

		//initial render call to tell bgfx to use the same thread as the window thread
		bgfx::renderFrame();

		glfwGetWindowSize(renderTargetWindow, &width, &height);

		//initialize bgfx (renderer)
		bgfx::Init init;
		init.type = bgfx::RendererType::Vulkan;
		init.platformData.nwh = glfwGetWin32Window(renderTargetWindow);
		init.resolution.width = width;
		init.resolution.height = height;
		init.resolution.reset = BGFX_RESET_VSYNC;
		GraphicsCallback graphicsCallback;
		init.callback = &graphicsCallback;

		if (!bgfx::init(init))
		{
			throw std::exception("Failed to initialize BGFX");
		}

		//set the background color and enable debug text
		bgfx::setDebug(BGFX_DEBUG_TEXT);
		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x008000ff, 1.0f, 0);

		//loader shaders
		std::cout << "VERTEX BEGIN" << std::endl;
		vertexShader = LoadShader("shaders\\vert.bin");
		if (vertexShader.idx == bgfx::kInvalidHandle)
		{
			throw std::exception("Vertex shader failed to create");
		}
		std::cout << "VERTEX END" << std::endl;

		std::cout << "FRAG BEGIN" << std::endl;
		fragmentShader = LoadShader("shaders\\frag.bin");
		if (fragmentShader.idx == bgfx::kInvalidHandle)
		{
			throw std::exception("Fragment shader failed to create");
		}
		std::cout << "FRAG END" << std::endl;

		std::cout << "PROGRAM BEGIN" << std::endl;
		programHandle = bgfx::createProgram(vertexShader, fragmentShader, true);
		if (programHandle.idx == bgfx::kInvalidHandle)
		{
			throw std::exception("Program failed to create");
		}
		std::cout << "PROGRAM END" << std::endl;

		entities = entitiesIn;
	}

	void Graphics::Update()
	{
		std::cout << "UPDATING" << std::endl;
		bgfx::dbgTextPrintf(0, 0, 0x0f, "Welcome to SekhmetEngine");
		bgfx::frame();
		std::cout << "DONE UPDATING" << std::endl;
	}

	void Graphics::Destroy()
	{
		std::cout << "Destroying Graphics" << std::endl;
		bgfx::shutdown();
	}

	void Graphics::ExecuteRange(enki::TaskSetPartition range_, uint32_t threadnum_)
	{
		bgfx::touch(0);
		Render();
		Update();
	}

	void Graphics::SetViewRectSize(int widthIn, int heightIn)
	{
		width = widthIn;
		height = heightIn;

		bgfx::setViewRect(0, 0, 0, width, height);
	}

	void Graphics::Render()
	{
		//Get component's model
		//const aiScene* scene
		StaticMeshComponent* staticMeshComponent = (StaticMeshComponent*)entities[0]->GetComponents()[0];
		const aiScene* scene = staticMeshComponent->GetModel();

		//foreach mesh in component
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			//Create buffer for position vertices
			if (scene->mMeshes[i]->HasPositions())
			{
				std::vector<glm::vec3> vertices;

				for (int j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
				{
					glm::vec3 vertex;
					vertex.x = scene->mMeshes[i]->mVertices[j].x;
					vertex.y = scene->mMeshes[i]->mVertices[j].y;
					vertex.z = scene->mMeshes[i]->mVertices[j].z;
					vertices.push_back(vertex);
				}

				//define the vertex layout in the obj and create vertex buffer handle
				bgfx::VertexLayout vertexLayout;
				vertexLayout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).end();
				vertexBufferHandle = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size()), vertexLayout);

				//submit to pipeline for rendering
				bgfx::setVertexBuffer(0, vertexBufferHandle);
			}

			//create buffer for indices
			if (scene->mMeshes[i]->HasFaces())
			{	
				std::vector<int> vertexIndices, uvIndices, normalIndices;
				for (int j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
				{
					vertexIndices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[0]);
					uvIndices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[1]);
					normalIndices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[2]);
				}

				//define the vertex layout in the obj and create vertex buffer handle
				indexBufferHandle[0] = bgfx::createIndexBuffer(bgfx::makeRef(vertexIndices.data(), vertexIndices.size()));
				indexBufferHandle[1] = bgfx::createIndexBuffer(bgfx::makeRef(uvIndices.data(), uvIndices.size()));
				indexBufferHandle[2] = bgfx::createIndexBuffer(bgfx::makeRef(normalIndices.data(), normalIndices.size()));

				//submit to pipeline for rendering
				bgfx::setIndexBuffer(indexBufferHandle[0]);

				std::cout << vertexIndices.size() << std::endl;
			}

			bgfx::setState(BGFX_STATE_DEFAULT);
			bgfx::submit(0, programHandle);
		}
	}

	bgfx::ShaderHandle Graphics::LoadShader(std::string shaderPath)
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
		const bgfx::Memory* shaderMemory = bgfx::copy(shaderBinaryData.data(), fileSize + 1);

		std::cout << "SIZE " << shaderMemory->size << std::endl;
		//create a shader handler
		bgfx::ShaderHandle shaderHandle = bgfx::createShader(shaderMemory);

		shaderFileHandle.close();

		return shaderHandle;
	}
}