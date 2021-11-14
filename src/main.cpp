#include "engine.h"

int main(int argc, char* args[]){
    Engine *engine = new Engine();

    if(engine->InitSDL()){
        while(engine->IsRunning()){
            engine->CheckInput();
            engine->RenderScreen();
            engine->UpdateFramesPerSecond();
        }
    }

    engine->CloseSDL();
    delete engine;
    return 0;
}
