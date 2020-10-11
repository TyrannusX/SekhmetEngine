/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include <thread>
#include <iostream>
#include <fstream>
#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <json/json.hpp>
#include "Engine/Engine.h"
#include "Components/ComponentTypes.h"
#include "Components/StaticMeshComponent.h"

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
		//initialize task scheduler
		std::cout << "Initializing Main Engine\n";
		taskScheduler = new enki::TaskScheduler();
		taskScheduler->Initialize();

		//create entities with entities.json
		std::ifstream jsonFileHandle("entities.json");
		std::string temp;
		std::string jsonContent;
		while (std::getline(jsonFileHandle, temp))
		{
			jsonContent += temp;
		}
		nlohmann::json entitiesJson = nlohmann::json::parse(jsonContent);
		auto entitiesArray = entitiesJson["entities"];
		for (auto& entity : entitiesArray)
		{
			//create entity
			Entity* nextEntity = new Entity();
			nextEntity->Initialize(entity["name"]);

			//create and attach components to entity
			auto componentsArray = entity["components"];
			for (auto& component : componentsArray)
			{
				int componentTypeToInitialize = component["type"];
				switch (componentTypeToInitialize)
				{
					case ComponentType::StaticMesh:
						StaticMeshComponent* staticMeshComponent = new StaticMeshComponent();
						staticMeshComponent->LoadModel((const std::string)component["meshPath"]);
						nextEntity->AddComponent(staticMeshComponent);
				}
			}

			//add entity to list
			entities.push_back(nextEntity);
		}
		

		gui = new Gui();
		gui->Initialize();

		graphics = new Graphics();
		graphics->Initialize(gui->GetWindow(), entities);
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