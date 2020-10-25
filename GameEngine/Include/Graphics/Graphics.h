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
	struct GraphicsCallback : public bgfx::CallbackI
	{
		virtual void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) override
		{
			std::cout << "BGFX FATAL ERROR " << _str << std::endl;
		}

		virtual void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) override
		{
		}

		virtual void profilerBegin(const char* /*_name*/, uint32_t /*_abgr*/, const char* /*_filePath*/, uint16_t /*_line*/) override
		{
		}

		virtual void profilerBeginLiteral(const char* /*_name*/, uint32_t /*_abgr*/, const char* /*_filePath*/, uint16_t /*_line*/) override
		{
		}

		virtual void profilerEnd() override
		{
		}

		virtual uint32_t cacheReadSize(uint64_t _id) override
		{
			return 0;
		}

		virtual bool cacheRead(uint64_t _id, void* _data, uint32_t _size) override
		{
			return true;
		}

		virtual void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) override
		{
		}

		virtual void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t /*_size*/, bool _yflip) override
		{
		}

		virtual void captureBegin(uint32_t _width, uint32_t _height, uint32_t /*_pitch*/, bgfx::TextureFormat::Enum /*_format*/, bool _yflip) override
		{
		}

		virtual void captureEnd() override
		{
		}

		virtual void captureFrame(const void* _data, uint32_t /*_size*/) override
		{
		}
	};

	class Graphics : public enki::ITaskSet
	{
		private:
			int width;
			int height;
			std::vector<Entity*> entities;
			bgfx::ShaderHandle vertexShader;
			bgfx::ShaderHandle fragmentShader;
			bgfx::ProgramHandle programHandle;
			bgfx::VertexBufferHandle vertexBufferHandle;
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