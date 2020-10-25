/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#include "Components/StaticMeshComponent.h"

namespace SekhmetEngine
{
	void StaticMeshComponent::LoadModel(const std::string& file)
	{
		tinyobj::LoadObj(&attributes, &shape, nullptr, nullptr, nullptr, file.c_str());
	}

	std::vector<tinyobj::shape_t> StaticMeshComponent::GetModel()
	{
		return shape;
	}

	tinyobj::attrib_t StaticMeshComponent::GetAttributes()
	{
		return attributes;
	}
}