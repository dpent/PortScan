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
#include <semaphore>
#include <queue>
#include <thread>
#include <mutex>
#include "workerThread.h"
#include "job.h"

class Engine {
    public:
    static GLFWwindow* window;
    static std::vector<Scan*> history;
    static char currentCommand[256];
    static int selectedScan;

	static std::counting_semaphore<INT_MAX> jobInQueueSem;
	static std::mutex jobQueueMutex;
    static std::mutex historyMutex;

    static std::unordered_map<std::thread::id, WorkerThread*> threadPool;
	static std::queue<Job*> jobQueue;

    static char spinner[];
    static int spinnerIndex;

    static float spinnerTimer;
    static const float spinnerInterval; // seconds between spinner updates

    Engine();
    void initIMGUI();
    void initWindow();
    void mainLoop();
    
    static void enqueueCommand(char* command);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void initThreadPool(uint16_t poolSize);
    static void killThreadPool();
};