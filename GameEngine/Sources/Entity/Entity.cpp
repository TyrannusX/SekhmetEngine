/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include "Entity/Entity.h"

namespace SekhmetEngine
{
	template<typename T>
	long long Entity<T>::nextId = 0;

	template<typename T>
	void Entity<T>::Initialize()
	{
	}

	template<typename T>
	void Entity<T>::AddComponent(T* component)
	{
		components.push_back(component);
	}

	template<typename T>
	void Entity<T>::RemoveComponent(int index)
	{
		components.erase(components.begin() + index);
	}
}