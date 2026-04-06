#include "../headers/engine.h"
#include <sstream>
#include "../../helper.h"
#include <cstring>

GLFWwindow* Engine::window = nullptr;
std::vector<Scan*> Engine::history = std::vector<Scan*>();
char Engine::currentCommand[256] = "\0";
int Engine::selectedScan = -1;

std::counting_semaphore<INT_MAX> Engine::jobInQueueSem(0);
std::mutex Engine::jobQueueMutex;
std::mutex Engine::historyMutex;

std::unordered_map<std::thread::id, WorkerThread*> Engine::threadPool;
std::queue<Job*> Engine::jobQueue;

char Engine::spinner[] = {'-', '\\', '|', '/'};
int Engine::spinnerIndex = 0;

float Engine::spinnerTimer = 0.0f;
const float Engine::spinnerInterval = 0.1f;

Engine::Engine(){
}

void Engine::initWindow(){
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    Engine::window = glfwCreateWindow(1440, 720, "PortScan GUI", nullptr, nullptr);
    if (!Engine::window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwSetKeyCallback(Engine::window, keyCallback);

    glfwMakeContextCurrent(Engine::window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return;
    }

    Engine::initThreadPool(4);

    Engine::initIMGUI();

};

void Engine::mainLoop(){
    while (!glfwWindowShouldClose(Engine::window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport();
        WindowManager::createCommandWindow();
        WindowManager::createPastScansWindow();
        WindowManager::createHelpWindow();
        WindowManager::createOutputWindow();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(Engine::window);
        glfwPollEvents();
    }

    Engine::killThreadPool();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(Engine::window);
    glfwTerminate();

    return;
}

void Engine::initIMGUI(){
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(Engine::window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void Engine::enqueueCommand(char* command){
    
    Engine::jobQueueMutex.lock();
    ScanJob* job = new ScanJob(std::string(command));
    Engine::jobQueue.push(job);
    Engine::jobQueueMutex.unlock();
    Engine::jobInQueueSem.release();

    Scan::activeScansMutex.lock();
    Scan::numActiveScans++;
    Scan::activeScansMutex.unlock();
}

void Engine::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

    if (key == GLFW_KEY_UP) {
        Engine::historyMutex.lock();
        if (!history.empty()) {
            selectedScan = (selectedScan - 1 + history.size()) % history.size();
            std::strcpy(Engine::currentCommand, Engine::history[Engine::selectedScan]->command.c_str());
        }
        Engine::historyMutex.unlock();
    } 
    else if (key == GLFW_KEY_DOWN) {
        Engine::historyMutex.lock();
        if (!history.empty()) {
            selectedScan = (selectedScan + 1 + history.size()) % history.size();
            std::strcpy(Engine::currentCommand, Engine::history[Engine::selectedScan]->command.c_str());
        }
        Engine::historyMutex.unlock();
    }
}

void Engine::initThreadPool(uint16_t poolSize) {

    for (uint16_t i = 0; i < poolSize; i++) {

        WorkerThread* wt = new WorkerThread();

        Engine::threadPool[wt->id] = wt;
    }
}


void Engine::killThreadPool() {

    Engine::jobQueueMutex.lock();

    while (!Engine::jobQueue.empty()) {
        Engine::jobQueue.pop();
    }

    for (size_t i = 0; i < Engine::threadPool.size(); i++) {
    
        PrepareForJoinJob* j = new PrepareForJoinJob();
        Engine::jobQueue.push(j);
    }

    Engine::jobQueueMutex.unlock();

    for (const auto& pair : Engine::threadPool) {
        pair.second->alive = false;
    }

    for (size_t i = 0; i < Engine::threadPool.size(); i++) {
        Engine::jobInQueueSem.release();
    }

    int j = 0;
    for (const auto& pair : Engine::threadPool) {
        if (pair.second->thread.joinable()) {
            j++;
            pair.second->thread.join();
        }
    }

    for (const auto& pair : Engine::threadPool) {
        delete pair.second;
    }

}