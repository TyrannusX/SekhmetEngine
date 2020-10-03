#include <thread>
#include <iostream>
#include <bgfx/bgfx.h>
#include "Graphics/Graphics.h"
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
		subSystems.push_back(gui);

		Graphics* graphics = new Graphics();
		graphics->Initialize(gui->GetWindow());
		subSystems.push_back(graphics);
	}

	void Engine::Destroy()
	{
		std::cout << "Destroying Main Engine\n";
		for (std::vector<Updateable*>::iterator iter = subSystems.begin(); iter != subSystems.end(); ++iter)
		{
			(*iter)->PublishEvent(Event::Destroy);
		}
	}

	void Engine::Run()
	{
		while (!gui->IsExited())
		{
			for (std::vector<Updateable*>::iterator iter = subSystems.begin(); iter != subSystems.end(); ++iter)
			{
				(*iter)->PublishEvent(Event::Update);
			}
		}
	}
}