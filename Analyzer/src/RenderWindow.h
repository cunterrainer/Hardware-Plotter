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
private:
	GLFWwindow* m_Window = nullptr;

	bool m_UsingVsync       = true;
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
public:
	RenderWindow(int width = 1600, int height = 920, const char* title = "Analyzer", GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL) noexcept;
	~RenderWindow() noexcept;

	// loop
	inline bool IsOpen()     const noexcept { return !glfwWindowShouldClose(m_Window);      }
	inline void EndFrame()   const noexcept { Clear(); ImGuiRender(); PollEvents(); Swap(); }
	inline ImVec2 Size() const noexcept { const ImGuiIO& io = ImGui::GetIO(); return { io.DisplaySize.x, io.DisplaySize.y }; }
	void Show(bool connected, PortSetup* portSetup) noexcept;

	inline bool& SaveAllClicked()         noexcept { return m_SaveAllClicked;           }
	inline bool  ConnectClicked()   const noexcept { return m_ConnectClicked;           }
	inline bool  ShowDebugInfo()    const noexcept { return m_DebugInfoChecked;         }
	inline void  ResetConnectClicked()    noexcept { m_ConnectClicked = false;          }

	static void SetButtonRed(bool condition) noexcept;
	static void ResetButtonColor() noexcept;
	static void SetThemePopup() noexcept;
	static void SetThemeWindow() noexcept;
};

int MsgBoxError(const char* message);