/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <tinyobjloader/tiny_obj_loader.h>

namespace SekhmetEngine
{
	class StaticMeshComponent
	{
		private:
			std::vector<tinyobj::shape_t> shape;
			tinyobj::attrib_t attributes;
		public:
			void LoadModel(const std::string& file);
			std::vector<tinyobj::shape_t> GetModel();
			tinyobj::attrib_t GetAttributes();
	};
}