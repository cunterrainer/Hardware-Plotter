#pragma once
#include <array>
#include <utility>

#include "GLFW/glfw3.h"
#include "ImGui/imgui.h"

#define IMGUI_WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar

class Window
{
private:
	static constexpr ImVec4 ColorRed = { 0.9f, 0.072f, 0.072f, 1.f };
	static constexpr ImVec4 ColorLightRed = { 0.9f, 0.372f, 0.372f, 1.f };
	static constexpr ImVec4 ColorDark = { 0.27f, 0.27f, 0.27f, 1.f };
	static constexpr ImVec4 ColorGrey = { 0.57f, 0.57f, 0.57f, 1.f };
	static constexpr ImVec4 ColorWhite = { 1.f,   1.f,   1.f, 1.f };
	static inline const std::array<std::pair<int, std::reference_wrapper<const ImVec4>>, 9> StyleColors{
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_FrameBg,        std::ref(ColorDark)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_FrameBgHovered, std::ref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_CheckMark,      std::ref(ColorWhite)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_Header,         std::ref(ColorDark)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_HeaderActive,   std::ref(ColorDark)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_HeaderHovered,  std::ref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_Button,         std::ref(ColorDark)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_ButtonHovered,  std::ref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_ButtonActive,   std::ref(ColorGrey)),
	};
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
	inline void StartFrame() const noexcept { ImGuiStartFrame();                       }
	inline void EndFrame()   const noexcept { Clear(); ImGuiRender(); PollEvents(); Swap(); }

	ImVec2 GetSize() const noexcept;

	// ImGui
	void ImGuiInit(const char* iniFileName = nullptr) const noexcept;
	void ImGuiStartFrame() const noexcept;
	void ImGuiRender() const noexcept;
	void ImGuiPushGlobalStyle() const noexcept;

	static void PushRedButtonColors(bool condition) noexcept;
	static void Window::PopRedButtonColors() noexcept;
};

int MsgBoxError(const char* message);