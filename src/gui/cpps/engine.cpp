#include "../headers/engine.h"
#include <sstream>
#include "../../helper.h"

GLFWwindow* Engine::window = nullptr;
std::vector<Scan*> Engine::history = std::vector<Scan*>();
char Engine::currentCommand[256] = "\0";
int Engine::selectedScan = -1;

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

    glfwMakeContextCurrent(Engine::window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return;
    }

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

void Engine::executeCommand(char* command){
    std::string commandStr(command);

    Scan* newScan = new Scan(commandStr);
    Engine::history.push_back(newScan);

    std::istringstream iss(commandStr);
    std::vector<std::string> tokens;
    std::string token;
    tokens.push_back("portscan");
    while (iss >> token) {
        tokens.push_back(token);
    }

    // Prepare argc and argv
    int argc = static_cast<int>(tokens.size());
    std::vector<char*> argv;
    for (auto& s : tokens) {
        argv.push_back(const_cast<char*>(s.c_str()));
    }

    newScan->rawResults = Helper::portscan(argc, argv.data());
    newScan->formatOutputString();
    Engine::selectedScan = Engine::history.size() - 1;
}