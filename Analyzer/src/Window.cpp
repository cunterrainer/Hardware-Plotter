#include <Windows.h>

#include "GLFW/glfw3.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImPlot/implot.h"

#include "Window.h"
#include "Arial.h"
#include "Log.h"

Window::Window(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) noexcept
{
    glfwSetErrorCallback([](int error, const char* description){ Err << "[GLFW] " << error << ": " << description << Endl; });

    if (!glfwInit())
    {
        Err << "Failed to initialize GLFW" << Endl;
        return;
    }
    Log << "Initialized GLFW" << Endl;

    // Create window with graphics context
    m_Window = glfwCreateWindow(width, height, title, monitor, share);
    if (m_Window == NULL)
    {
        Err << "Failed to create window w: " << width << " h : " << height << " t : " << title << Endl;
        return;
    }
    Log << "Created window w: " << width << " h: " << height << " t: " << title << Endl;

    // center the window
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(m_Window, (mode->width - width) / 2, (mode->height - height) / 2);

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);
    glClearColor(0.27f, 0.27f, 0.27f, 1.0f);
    if (!ImGuiInit()) return;
    ImGuiPushGlobalStyle();
    ImPlot::CreateContext();
    ImPlot::StyleColorsClassic();
    Log << "Initialized ImPlot" << Endl;
}


Window::~Window() noexcept
{
    // Cleanup
    ImPlot::DestroyContext();
    Log << "Shut down ImPlot" << Endl;

    ImGui::PopStyleVar(); // ImGuiPushGlobalStyle()
    ImGui::PopStyleColor((int)StyleColors.size());
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    Log << "Shut down ImGui" << Endl;

    glfwDestroyWindow(m_Window);
    glfwTerminate();
    Log << "Shut down GLFW" << Endl;
}


ImVec2 Window::GetSize() const noexcept
{
    const ImGuiIO& io = ImGui::GetIO();
    return { io.DisplaySize.x, io.DisplaySize.y };
}


bool Window::ImGuiInit(const char* iniFileName) const noexcept
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryCompressedTTF(sg_ArialCompressedData, sg_ArialCompressedSize, 19);
    io.IniFilename = iniFileName;

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    if (!ImGui_ImplOpenGL3_Init("#version 130"))
    {
        Err << "Failed to initialize ImGui OpenGL3 implementation" << Endl;
        return false;
    }
    ImGui::StyleColorsDark();
    Log << "Initialized ImGui" << Endl;
    return true;
}


void Window::ImGuiStartFrame() const noexcept
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}


void Window::ImGuiRender() const noexcept
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::ImGuiPushGlobalStyle() const noexcept
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    for (auto& [colorFlag, color] : StyleColors)
        ImGui::PushStyleColor(colorFlag, color.get());
}

void Window::PushRedButtonColors(bool condition) noexcept
{
    if (condition)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, Window::ColorRed);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Window::ColorLightRed);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Window::ColorLightRed);
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, Window::ColorDark);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Window::ColorGrey);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Window::ColorGrey);
    }
}

void Window::PopRedButtonColors() noexcept
{
    ImGui::PopStyleColor(3);
}

int MsgBoxError(const char* message) { return MessageBoxA(NULL, message, "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL); }