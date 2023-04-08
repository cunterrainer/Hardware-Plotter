#pragma once
#include <array>
#include <utility>
#include <functional>

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
	static inline const std::array<std::pair<int, std::reference_wrapper<const ImVec4>>, 10> StyleColors{
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_FrameBg,        std::cref(ColorDark)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_FrameBgHovered, std::cref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_CheckMark,      std::cref(ColorWhite)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_Header,         std::cref(ColorDark)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_HeaderActive,   std::cref(ColorDark)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_HeaderHovered,  std::cref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_Button,         std::cref(ColorDark)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_ButtonHovered,  std::cref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_ButtonActive,   std::cref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_TitleBgActive,  std::cref(ColorDark))
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
	bool ImGuiInit(const char* iniFileName = nullptr) const noexcept;
	void ImGuiStartFrame() const noexcept;
	void ImGuiRender() const noexcept;
	void ImGuiPushGlobalStyle() const noexcept;

	static void PushRedButtonColors(bool condition) noexcept;
	static void PopRedButtonColors() noexcept;
};

int MsgBoxError(const char* message);