/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#include <enkiTS/TaskScheduler.h>
#include "Graphics/Graphics.h"
#include "Gui/Gui.h"

namespace SekhmetEngine
{
    class Engine
    {
    private:
        enki::TaskScheduler* taskScheduler;
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