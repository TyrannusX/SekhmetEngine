/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include <fstream>
#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <bgfx/platform.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "Graphics/Graphics.h"
#include "Graphics/Vertex.h"

namespace SekhmetEngine
{
	void Graphics::Initialize(GLFWwindow* renderTargetWindow, std::vector<Entity*> entitiesIn)
	{
		//initial render call to tell bgfx to use the same thread as the window thread
		bgfx::renderFrame();

		glfwGetWindowSize(renderTargetWindow, &width, &height);

		//initialize bgfx (renderer)
		bgfx::Init init;
		init.type = bgfx::RendererType::OpenGL;
		init.platformData.nwh = glfwGetWin32Window(renderTargetWindow);
		init.resolution.width = width;
		init.resolution.height = height;
		init.resolution.reset = BGFX_RESET_VSYNC;
		/*GraphicsCallback graphicsCallback;
		init.callback = &graphicsCallback;*/

		if (!bgfx::init(init))
		{
			throw std::exception("Failed to initialize BGFX");
		}

		//set the background color and enable debug text
		bgfx::setDebug(BGFX_DEBUG_TEXT);
		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xfffffff, 1.0f, 0);
		bgfx::setViewRect(0, 0, 0, width, height);

		//loader shaders
		vertexShader = LoadShader("C:\\code\\c++\\game-engine\\GameEngine\\x64\\Debug\\shaders\\vert.bin");
		if (vertexShader.idx == bgfx::kInvalidHandle)
		{
			throw std::exception("Vertex shader failed to create");
		}

		fragmentShader = LoadShader("C:\\code\\c++\\game-engine\\GameEngine\\x64\\Debug\\shaders\\frag.bin");
		if (fragmentShader.idx == bgfx::kInvalidHandle)
		{
			throw std::exception("Fragment shader failed to create");
		}

		programHandle = bgfx::createProgram(vertexShader, fragmentShader, true);
		if (programHandle.idx == bgfx::kInvalidHandle)
		{
			throw std::exception("Program failed to create");
		}

		entities = entitiesIn;
	}

	void Graphics::Update()
	{
		bgfx::setViewRect(0, 0, 0, width, height);
		bgfx::setViewTransform(0, 0, 0);
		bgfx::touch(0);

		bgfx::setVertexBuffer(0, vertexBufferHandle);
		bgfx::setIndexBuffer(indexBufferHandle[0]);
		bgfx::setState(BGFX_STATE_DEFAULT);
		bgfx::submit(0, programHandle);
		bgfx::dbgTextPrintf(0, 0, 0xF5DE91f, "Welcome to SekhmetEngine");
		bgfx::frame();
	}

	void Graphics::Destroy()
	{
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
		std::vector<Vertex> vertices;
		std::vector<int> indices;

		//foreach mesh in component
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			//store the positions, normals, and texture coordinates
			if (scene->mMeshes[i]->HasPositions())
			{
				for (int j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
				{
					glm::vec3 position;
					position.x = scene->mMeshes[i]->mVertices[j].x;
					position.y = scene->mMeshes[i]->mVertices[j].y;
					position.z = scene->mMeshes[i]->mVertices[j].z;

					glm::vec3 normal;
					normal.x = scene->mMeshes[i]->mNormals[j].x;
					normal.y = scene->mMeshes[i]->mNormals[j].y;
					normal.z = scene->mMeshes[i]->mNormals[j].z;

					glm::vec2 textureCoordinate;
					if (scene->mMeshes[i]->mTextureCoords[0])
					{
						textureCoordinate.x = scene->mMeshes[i]->mTextureCoords[0][j].x;
						textureCoordinate.y = scene->mMeshes[i]->mTextureCoords[0][j].y;
					}
					else
					{
						textureCoordinate.x = 0.0f;
						textureCoordinate.y = 0.0f;
					}

					Vertex vertex;
					vertex.Position = position;
					vertex.Normal = normal;
					vertex.TextureCoordinates = textureCoordinate;
					vertices.push_back(vertex);
				}
			}

			//store the face indices
			if (scene->mMeshes[i]->HasFaces())
			{	
				for (int j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
				{
					for (int k = 0; k < scene->mMeshes[i]->mFaces[j].mNumIndices; k++)
					{
						indices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[k]);
					}
				}
			}

			bgfx::VertexLayout vertexLayout;
			vertexLayout.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();

			vertexBufferHandle = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size()), vertexLayout);
			indexBufferHandle[0] = bgfx::createIndexBuffer(bgfx::makeRef(indices.data(), indices.size()));
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

		//create a shader handler
		bgfx::ShaderHandle shaderHandle = bgfx::createShader(shaderMemory);

		shaderFileHandle.close();

		return shaderHandle;
	}
}