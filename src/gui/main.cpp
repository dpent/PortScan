#include "../helper.h"
#include "headers/engine.h"

int main() {
    Engine* engine = new Engine();
    engine->initWindow();
    engine->mainLoop();

    delete engine;

    return 0;
}