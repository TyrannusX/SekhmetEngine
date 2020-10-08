/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/
#include <iostream>
#include <mimalloc/mimalloc.h>
#include <mimalloc/mimalloc-override.h>
#include <mimalloc/mimalloc-new-delete.h>
#include <enkiTS/TaskScheduler.h>
#include "Engine/Engine.h"
using namespace std;

int main(int argc, char** argv)
{
    //Initialize engine and task scheduler
    enki::TaskScheduler* taskScheduler = new enki::TaskScheduler();
    taskScheduler->Initialize();
    SekhmetEngine::Engine* engine = new SekhmetEngine::Engine();
    engine->Initialize();

    //spawn main thread (it all begins here)
    taskScheduler->AddTaskSetToPipe(engine);
    taskScheduler->WaitforTask(engine, enki::TASK_PRIORITY_HIGH);

    //cleanup
    engine->Destroy();
    delete engine;
    delete taskScheduler;
    return 0;
}