/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include <thread>
#include <iostream>
#include <bgfx/bgfx.h>
#include "Engine/Engine.h"

namespace SekhmetEngine
{
	Engine::Engine()
	{

	}

	Engine::~Engine()
	{

	}

	void Engine::Initialize()
	{
		std::cout << "Initializing Main Engine\n";
		taskScheduler = new enki::TaskScheduler();
		taskScheduler->Initialize();

		gui = new Gui();
		gui->Initialize();

		graphics = new Graphics();
		graphics->Initialize(gui->GetWindow());
	}

	void Engine::Destroy()
	{
		std::cout << "Destroying Main Engine\n";
		gui->Destroy();
		graphics->Destroy();
	}

	void Engine::Run()
	{
		while (!gui->IsExited())
		{
			gui->Update();
			graphics->Update();
		}
	}

	void Engine::ExecuteRange(enki::TaskSetPartition range_, uint32_t threadnum_)
	{
		Run();
	}
}