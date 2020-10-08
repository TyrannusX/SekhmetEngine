/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#pragma once
#include <vector>
#include <enkiTS/TaskScheduler.h>
#include "Graphics/Graphics.h"
#include "Gui/Gui.h"

namespace SekhmetEngine
{
    class Engine : public enki::ITaskSet
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
        void ExecuteRange(enki::TaskSetPartition range_, uint32_t threadnum_) override;
    };
}