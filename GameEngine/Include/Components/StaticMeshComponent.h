/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace SekhmetEngine
{
	class StaticMeshComponent
	{
		private:
			const aiScene* staticMesh;
		public:
			void LoadModel(const std::string& file);
			const aiScene* GetModel();
	};
}