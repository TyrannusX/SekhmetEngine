/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#include <vector>
#include <string>

namespace SekhmetEngine
{
	class Entity
	{
		private:
			static long long nextId;
			std::string entityName;
			std::vector<void*>	components;
			long long id;
		public:
			void Initialize(std::string entityNameIn);
			void AddComponent(void* component);
			void RemoveComponent(int index);
			std::vector<void*> GetComponents();
	};
}