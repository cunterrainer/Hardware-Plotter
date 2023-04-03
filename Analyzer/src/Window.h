#pragma once
#include "GLFW/glfw3.h"

#define IMGUI_WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar

class Window
{
private:
	GLFWwindow* m_Window = nullptr;
public:
	Window(int width = 1600, int height = 920, const char* title = "Window", GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL);
	~Window();

	// loop
	inline bool IsOpen()     const { return !glfwWindowShouldClose(m_Window); }
	inline void Swap()       const { glfwSwapBuffers(m_Window);               }
	inline void Clear()      const { glClear(GL_COLOR_BUFFER_BIT);            }
	inline void PollEvents() const { glfwPollEvents();                        }
	inline void WaitEvents() const { glfwWaitEvents();                        }
	inline void StartFrame() const { Clear(); ImGuiStartFrame();              }
	inline void EndFrame()   const { ImGuiRender(); PollEvents(); Swap();     }

	// ImGui
	void ImGuiInit(const char* iniFileName = nullptr) const;
	void ImGuiStartFrame() const;
	void ImGuiRender() const;
};