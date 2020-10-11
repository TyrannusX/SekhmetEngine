/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
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

				const bgfx::VertexLayout vertexLayout;
				bgfx::createVertexLayout(vertexLayout);
				bgfx::createVertexBuffer(bgfx::makeRef(positionVertices, sizeof(positionVertices)), vertexLayout);
			}
		}
	}
}