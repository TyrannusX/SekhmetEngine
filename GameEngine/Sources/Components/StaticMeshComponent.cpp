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
		Assimp::Importer* importer = new Assimp::Importer();
		staticMesh = importer->ReadFile(file, 0);
	}
}