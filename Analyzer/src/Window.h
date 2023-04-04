#pragma once
#include "GLFW/glfw3.h"

#define IMGUI_WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar

class Window
{
private:
	GLFWwindow* m_Window = nullptr;
public:
	Window(int width = 1600, int height = 920, const char* title = "Analyzer", GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL) noexcept;
	~Window() noexcept;

	// loop
	inline bool IsOpen()     const noexcept { return !glfwWindowShouldClose(m_Window); }
	inline void Swap()       const noexcept { glfwSwapBuffers(m_Window);               }
	inline void Clear()      const noexcept { glClear(GL_COLOR_BUFFER_BIT);            }
	inline void PollEvents() const noexcept { glfwPollEvents();                        }
	inline void WaitEvents() const noexcept { glfwWaitEvents();                        }
	inline void StartFrame() const noexcept { Clear(); ImGuiStartFrame();              }
	inline void EndFrame()   const noexcept { ImGuiRender(); PollEvents(); Swap();     }

	ImVec2 GetSize() const noexcept;

	// ImGui
	void ImGuiInit(const char* iniFileName = nullptr) const noexcept;
	void ImGuiStartFrame() const noexcept;
	void ImGuiRender() const noexcept;
};

int MsgBoxError(const char* message);