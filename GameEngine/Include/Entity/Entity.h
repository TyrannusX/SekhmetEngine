/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#include <vector>

namespace SekhmetEngine
{
	template<typename T>
	class Entity
	{
		private:
			static long long nextId;
			std::vector<T*>	components;
			long long id;
		public:
			void Initialize();
			void AddComponent(T* component);
			void RemoveComponent(int index);
	};
}