#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

class Engine {
    public:
    static GLFWwindow* window;

    Engine();
    void initIMGUI();
    void initWindow();
    void mainLoop();
};