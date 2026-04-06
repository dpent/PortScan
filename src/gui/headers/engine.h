#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "windowManager.h"
#include <vector>
#include "scan.h"

class Engine {
    public:
    static GLFWwindow* window;
    static std::vector<Scan*> history;
    static char currentCommand[256];
    static int selectedScan;

    Engine();
    void initIMGUI();
    void initWindow();
    void mainLoop();
    
    static void executeCommand(char* command);
};