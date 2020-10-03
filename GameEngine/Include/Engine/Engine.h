#pragma once
#include <vector>
#include <enkiTS/TaskScheduler.h>
#include "System/Updateable.h"
#include "Gui/Gui.h"

namespace SekhmetEngine
{
    class Engine
    {
    private:
        std::vector<Updateable*> subSystems;
        enki::TaskScheduler* taskScheduler;
        Gui* gui;
    public:
        Engine();
        ~Engine();
        void Initialize();
        void Destroy();
        void Run();
    };
}