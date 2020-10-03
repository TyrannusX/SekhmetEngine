#include <iostream>
#include <mimalloc/mimalloc.h>
#include <mimalloc/mimalloc-override.h>
#include <mimalloc/mimalloc-new-delete.h>
#include "Engine/Engine.h"
using namespace std;

int main(int argc, char** argv)
{
    SekhmetEngine::Engine* engine = new SekhmetEngine::Engine();
    engine->Initialize();
    engine->Run();
    engine->Destroy();
    delete engine;
    return 0;
}