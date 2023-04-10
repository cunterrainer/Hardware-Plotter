#pragma once
#include <array>
#include <utility>
#include <functional>

#include "GLFW/glfw3.h"
#include "ImGui/imgui.h"

#define IMGUI_WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar

class RenderWindow
{
private:
	static constexpr ImVec4 ColorRed = { 0.9f, 0.072f, 0.072f, 1.f };
	static constexpr ImVec4 ColorLightRed = { 0.9f, 0.372f, 0.372f, 1.f };
	static constexpr ImVec4 ColorDarkGrey = { 0.27f, 0.27f, 0.27f, 1.f };
	static constexpr ImVec4 ColorDark = { 0.17f, 0.17f, 0.17f, 1.f };
	static constexpr ImVec4 ColorGrey = { 0.57f, 0.57f, 0.57f, 1.f };
	static constexpr ImVec4 ColorWhite = { 1.f,   1.f,   1.f, 1.f };
	static inline const std::array<std::pair<int, std::reference_wrapper<const ImVec4>>, 10> StyleColors{
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_FrameBg,        std::cref(ColorDarkGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_FrameBgHovered, std::cref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_CheckMark,      std::cref(ColorWhite)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_Header,         std::cref(ColorDarkGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_HeaderActive,   std::cref(ColorDarkGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_HeaderHovered,  std::cref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_Button,         std::cref(ColorDarkGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_ButtonHovered,  std::cref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_ButtonActive,   std::cref(ColorGrey)),
			std::pair<int, std::reference_wrapper<const ImVec4>>(ImGuiCol_TitleBgActive,  std::cref(ColorDark))
	};
private:
	GLFWwindow* m_Window = nullptr;
private:
	inline void Swap()       const noexcept { glfwSwapBuffers(m_Window);    }
	inline void Clear()      const noexcept { glClear(GL_COLOR_BUFFER_BIT); }
	inline void PollEvents() const noexcept { glfwPollEvents();             }
	inline void WaitEvents() const noexcept { glfwWaitEvents();             }

	bool ImGuiInit(const char* iniFileName = nullptr) const noexcept;
	void ImGuiStartFrame() const noexcept;
	void ImGuiRender() const noexcept;
	void ImGuiSetTheme() const noexcept;
public:
	RenderWindow(int width = 1600, int height = 920, const char* title = "Analyzer", GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL) noexcept;
	~RenderWindow() noexcept;

	// loop
	inline bool IsOpen()     const noexcept { return !glfwWindowShouldClose(m_Window);      }
	inline void StartFrame() const noexcept { ImGuiStartFrame();                            }
	inline void EndFrame()   const noexcept { Clear(); ImGuiRender(); PollEvents(); Swap(); }
	inline ImVec2 Size() const noexcept { const ImGuiIO& io = ImGui::GetIO(); return { io.DisplaySize.x, io.DisplaySize.y }; }

	static void SetButtonRed(bool condition) noexcept;
	static void ResetButtonColor() noexcept;
	static void SetThemePopup() noexcept;
	static void SetThemeWindow() noexcept;
};

int MsgBoxError(const char* message);