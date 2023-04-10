#pragma once
#include <array>
#include <utility>
#include <functional>

#include "GLFW/glfw3.h"
#include "ImGui/imgui.h"

#include "Serial.h"
#include "PortSetup.h"

#define IMGUI_WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar

class RenderWindow
{
public:
	static constexpr ImVec2 BtnSize = { 150, 0 };
	static constexpr float  SettingsHeight  = 43;
private:
	static constexpr ImVec4 ColorRed      = {  0.9f, 0.072f, 0.072f, 1.f };
	static constexpr ImVec4 ColorLightRed = {  0.9f, 0.372f, 0.372f, 1.f };
	static constexpr ImVec4 ColorDarkGrey = { 0.27f,  0.27f,  0.27f, 1.f };
	static constexpr ImVec4 ColorDark     = { 0.17f,  0.17f,  0.17f, 1.f };
	static constexpr ImVec4 ColorGrey     = { 0.57f,  0.57f,  0.57f, 1.f };
	static constexpr ImVec4 ColorWhite    = {   1.f,    1.f,    1.f, 1.f };
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
	std::vector<Serial::Port> m_Ports = Serial::PortListener::GetPorts();
	std::string m_PortsString;

	bool m_SaveAllClicked   = false;
	bool m_ConnectClicked   = false;
	bool m_DebugInfoChecked = false;
private:
	inline void Swap()       const noexcept { glfwSwapBuffers(m_Window);    }
	inline void Clear()      const noexcept { glClear(GL_COLOR_BUFFER_BIT); }
	inline void PollEvents() const noexcept { glfwPollEvents();             }
	inline void WaitEvents() const noexcept { glfwWaitEvents();             }

	bool ImGuiInit(const char* iniFileName = nullptr) const noexcept;
	void ImGuiStartFrame() const noexcept;
	void ImGuiRender() const noexcept;
	void ImGuiSetTheme() const noexcept;

	std::string BuildPortsString() const noexcept;
public:
	RenderWindow(int width = 1600, int height = 920, const char* title = "Analyzer", GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL) noexcept;
	~RenderWindow() noexcept;

	// loop
	inline bool IsOpen()     const noexcept { return !glfwWindowShouldClose(m_Window);      }
	inline void EndFrame()   const noexcept { Clear(); ImGuiRender(); PollEvents(); Swap(); }
	inline ImVec2 Size() const noexcept { const ImGuiIO& io = ImGui::GetIO(); return { io.DisplaySize.x, io.DisplaySize.y }; }
	void Show(bool connected) noexcept;

	inline bool& SaveAllClicked()       noexcept { return m_SaveAllClicked;   }
	inline bool  ConnectClicked() const noexcept { return m_ConnectClicked;   }
	inline bool  ShowDebugInfo()  const noexcept { return m_DebugInfoChecked; }
	inline void  ResetConnectClicked()  noexcept { m_ConnectClicked = false;  }
	inline size_t NumOfPorts()    const noexcept { return m_Ports.size();     }
	std::string  SelectedPort()   const          { return m_Ports[(size_t)PortSetup::SelectedPort].com; }

	static void SetButtonRed(bool condition) noexcept;
	static void ResetButtonColor() noexcept;
	static void SetThemePopup() noexcept;
	static void SetThemeWindow() noexcept;
};

int MsgBoxError(const char* message);