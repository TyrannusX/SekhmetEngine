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
    SekhmetEngine::Engine* engine = new SekhmetEngine::Engine();
    engine->Initialize();
    engine->Run();

    //cleanup
    engine->Destroy();
    delete engine;
    return 0;
}