/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#include <vector>
#include <enkiTS/TaskScheduler.h>
#include "Graphics/Graphics.h"
#include "Gui/Gui.h"
#include "Entity/Entity.h"

namespace SekhmetEngine
{
    class Engine
    {
    private:
        enki::TaskScheduler* taskScheduler;
        std::vector<Entity*> entities;
        Gui* gui;
        Graphics* graphics;
    public:
        Engine();
        ~Engine();
        void Initialize();
        void Destroy();
        void Run();
    };
}