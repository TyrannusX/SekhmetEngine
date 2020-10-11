/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include <fstream>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
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
		init.platformData.nwh = glfwGetWin32Window(renderTargetWindow);
		init.resolution.width = width;
		init.resolution.height = height;
		init.resolution.reset = BGFX_RESET_VSYNC;

		if (!bgfx::init(init))
		{
			throw std::exception("Failed to initialize BGFX");
		}

		//set the background color and enable debug text
		bgfx::setDebug(BGFX_DEBUG_TEXT);
		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x008000ff, 1.0f, 0);

		//loader shaders
		vertexShader = LoadShader("shaders\\vertex-shader.bin");
		std::cout << vertexShader.idx << std::endl;
		if (vertexShader.idx == bgfx::kInvalidHandle)
		{
			throw std::exception("Vertex shader failed to create");
		}

		fragmentShader = LoadShader("shaders\\fragment-shader.bin");
		std::cout << fragmentShader.idx << std::endl;
		if (fragmentShader.idx == bgfx::kInvalidHandle)
		{
			throw std::exception("Fragment shader failed to create");
		}

		programHandle = bgfx::createProgram(vertexShader, fragmentShader, true);
		std::cout << programHandle.idx << std::endl;
		if (programHandle.idx == bgfx::kInvalidHandle)
		{
			throw std::exception("Program failed to create");
		}

		entities = entitiesIn;
	}

	void Graphics::Update()
	{
		bgfx::touch(0);
		bgfx::dbgTextPrintf(0, 0, 0x0f, "Welcome to SekhmetEngine");
		bgfx::frame();
	}

	void Graphics::Destroy()
	{
		std::cout << "Destroying Graphics" << std::endl;
		bgfx::shutdown();
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
				float* positionVertices = new float[scene->mMeshes[i]->mNumVertices * 3];

				for (int j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
				{
					positionVertices[j * 3] = scene->mMeshes[i]->mVertices[j].x;
					positionVertices[j * 3 + 1] = scene->mMeshes[i]->mVertices[j].y;
					positionVertices[j * 3 + 2] = scene->mMeshes[i]->mVertices[j].z;
				}

				//define the vertex layout in the obj and create vertex buffer handle
				bgfx::VertexLayout vertexLayout;
				vertexLayout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
				bgfx::VertexBufferHandle vertexBufferHandle = bgfx::createVertexBuffer(bgfx::makeRef(positionVertices, sizeof(positionVertices)), vertexLayout);

				//submit to pipeline for rendering
				bgfx::setVertexBuffer(0, vertexBufferHandle);
				bgfx::submit(0, programHandle);
			}
		}
	}

	bgfx::ShaderHandle Graphics::LoadShader(std::string shaderPath)
	{
		//setup shader file handle with binary format
		std::ifstream shaderFileHandle(shaderPath, std::ios::binary);

		//get shader file size by seeking to end and resetting to beginning
		size_t fileSize;
		shaderFileHandle.seekg(0, std::ios::end);
		fileSize = shaderFileHandle.tellg();
		shaderFileHandle.seekg(0, std::ios::beg);

		//read the binary data
		char* shaderBinaryData = new char[2048];
		shaderFileHandle.read(shaderBinaryData, fileSize);

		//copy the binary data into a bgfx memory handler
		const bgfx::Memory* shaderMemory = bgfx::copy(shaderBinaryData, fileSize + 1);

		//create a shader handler
		bgfx::ShaderHandle shaderHandle = bgfx::createShader(shaderMemory);
		
		return shaderHandle;
	}
}