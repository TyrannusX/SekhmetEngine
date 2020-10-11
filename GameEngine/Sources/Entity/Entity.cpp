/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include "Entity/Entity.h"

namespace SekhmetEngine
{
	long long Entity::nextId = 0;

	void Entity::Initialize(std::string entityNameIn)
	{
	}

	void Entity::AddComponent(void* component)
	{
		components.push_back(component);
	}

	void Entity::RemoveComponent(int index)
	{
		components.erase(components.begin() + index);
	}

	std::vector<void*> Entity::GetComponents()
	{
		return components;
	}
}