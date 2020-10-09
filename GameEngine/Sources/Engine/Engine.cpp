/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include <thread>
#include <iostream>
#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
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
			//run GUI/OS Window on main thread
			gui->Update();

			//run other systems on separate threads
			taskScheduler->AddTaskSetToPipe(graphics);

			//wait for all to complete
			taskScheduler->WaitforTask(graphics);

			//Reconcile systems
			int updatedWidth;
			int updatedHeight;
			glfwGetWindowSize(gui->GetWindow(), &updatedWidth, &updatedHeight);
			graphics->SetViewRectSize(updatedWidth, updatedHeight);
		}
	}
}