#include "application.hpp"
#include "glm/fwd.hpp"

#include <cstdlib>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/vec2.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Vis {

Application::Application() {
    std::cout << "App -> VIS\n";
    std::cout << "App -> Init\n";
}
Application::~Application() {}

int Application::run() {
    std::cout << "App -> Start\n";

    glm::vec2 vec = {800, 600};

    glfwInit();

    GLFWwindow *window;
    window = glfwCreateWindow(vec.x, vec.y, "VIS", nullptr, nullptr);

    const char* glsl_version = "#version 460";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    std::cout << "OpenGL -> Version: " << glGetString(GL_VERSION) << '\n';

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // (void)io;
    io.ConfigFlags
        |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags
        |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    constexpr uint32_t image_width  = 800;
    constexpr uint32_t image_height = 600;

    uint32_t image[image_width * image_height] = {};
    for(size_t i = 0; i < image_width * image_height; ++i)
    {
        uint8_t* r = ((uint8_t*)&image[i]) + 0;
        uint8_t* g = ((uint8_t*)&image[i]) + 1;
        uint8_t* b = ((uint8_t*)&image[i]) + 2;
        uint8_t* a = ((uint8_t*)&image[i]) + 3;

        *r = 255;
        *g = 0;
        *b = 255;
        *a = 255;
    }

    uint32_t texture;
    glGenTextures(1, &texture);
    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image_width, image_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("App"))
            {
                if(ImGui::MenuItem("Item1")) {}
                if(ImGui::MenuItem("Item2", "CTRL+Y", false, false)) {
                } // Disabled item
                ImGui::Separator();
                if(ImGui::MenuItem("Item3")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        ImGui::Begin("View");
        ImGui::Text("pointer = %x", texture);
        ImGui::Text("size = %d x %d",image_width, image_height);
        ImGui::Image((void*)(intptr_t)texture, ImVec2(image_width, image_height));
        ImGui::End();

        ImGui::Begin("Info panel");
        ImGui::Text("Panel text.");
        ImGui::End();

        // Rendering
        ImGui::Render();
        // int display_w, display_h;
        // glfwGetFramebufferSize(window, &display_w, &display_h);
        // glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    std::cout << "App -> Exit success\n";
    return EXIT_SUCCESS;
}

} // namespace Vis
