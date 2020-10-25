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
		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
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
		//camera transform
		const bx::Vec3 at = { 0.0f, 0.0f,  0.0f };
		const bx::Vec3 eye = { 0.0f, 0.0f, 50.0f };
		float view[16];
		bx::mtxLookAt(view, eye, at);
		float proj[16];
		bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(0, view, proj);

		bgfx::setViewRect(0, 0, 0, width, height);
		bgfx::touch(0);

		//object transform
		float mtx[16];
		bx::mtxRotateY(mtx, 0.0f);
		mtx[12] = 0.0f;
		mtx[13] = 0.0f;
		mtx[14] = -50.0f;
		bgfx::setTransform(mtx);

		bgfx::setVertexBuffer(0, vertexBufferHandle);
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
		std::vector<tinyobj::shape_t> shape = staticMeshComponent->GetModel();
		tinyobj::attrib_t attributes = staticMeshComponent->GetAttributes();
		std::vector<float> vertices;
		std::vector<unsigned int> indices;

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

		bgfx::VertexLayout vertexLayout;
		vertexLayout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();

		vertexBufferHandle = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size()), vertexLayout);
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