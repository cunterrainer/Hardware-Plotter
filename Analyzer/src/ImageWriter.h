#pragma once
#include <filesystem>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "ImGui/imgui.h"
#include "nfd/nfd.h"

#include "Window.h"
#include "Image.h"
#include "Thread.h"

class ImageWriter
{
private:
    static constexpr ImVec2 PathButtonSize{ 100, 0 };
    static constexpr float PathButtonPadding = 20.f;
    static inline Image m_Image;
    static inline std::string m_Path = std::filesystem::current_path().string() + "\\plot.png";
    static inline std::string m_DisplayPath = m_Path;
    static inline bool m_UpscaleOnWrite = false;
private:
    static inline void SaveFileDialog()
    {
        nfdchar_t* savePath = NULL;
        nfdresult_t result = NFD_SaveDialog("png;jpg;jpeg", NULL, &savePath);
        if (result == NFD_OKAY)
        {
            m_Path = savePath;
            m_DisplayPath = m_Path;
            free(savePath);
        }
        else if (result == NFD_CANCEL)
            Log << "[NFD] User pressed cancel" << Endl;
        else // error opening the file
        {
            Err << "[NFD] Failed to open the dialog: " << NFD_GetError() << Endl;
            MsgBoxError(std::string("Failed to open file dialog: " + std::string(NFD_GetError())).c_str());
        }
    }

    static inline void WriteImage(int(*write_func)(const char*, int, int, int, const void*, int))
    {
        if (!write_func(m_Path.c_str(), m_Image.Width(), m_Image.Height(), Image::NumOfChannels, m_Image.Data(), m_Image.Width() * Image::NumOfChannels))
            Err << "Failed to write to file: " << m_Path << " w: " << m_Image.Width() << " h: " << m_Image.Height() << Endl;
    }

    static inline void WriteImage(int(*write_func)(const char*, int, int, int, const void*))
    {
        if (!write_func(m_Path.c_str(), m_Image.Width(), m_Image.Height(), Image::NumOfChannels, m_Image.Data()))
            Err << "Failed to write to file: " << m_Path << " w: " << m_Image.Width() << " h: " << m_Image.Height() << Endl;
    }

    static inline void SaveImageToFile()
    {
        std::filesystem::path path(m_Path);
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });

        if (m_UpscaleOnWrite)
            m_Image.ScaleUp();
        if (extension == ".jpg" || extension == ".jpeg")
            WriteImage(stbi_write_jpg);
        else if (extension == ".bmp")
            WriteImage(stbi_write_bmp);
        else if (extension == ".tga")
            WriteImage(stbi_write_tga);
        else if (extension == ".png")
            WriteImage(stbi_write_png);
        else
        {
            m_Path += ".png";
            WriteImage(stbi_write_png);
        }
        Reset();
    }
public:
    static inline bool SaveImage(ImVec2 size, ImVec2 pos)
    {
        m_Image.Create(size, pos);
        const float aspecRatio = size.x / size.y;
        const ImVec2 imageSize{ size.y / 1.7f * aspecRatio, size.y / 1.7f };
        ImVec2 pathWidth = ImGui::CalcTextSize(m_DisplayPath.c_str());
        ImVec2 windowSize{ imageSize.x + pathWidth.x + PathButtonSize.x + PathButtonPadding, imageSize.y + 25 /*Title height*/ };
        if (windowSize.x >= size.x)
        {
            while (m_DisplayPath.size() > 4 && windowSize.x >= size.x)
            {
                m_DisplayPath.pop_back();
                pathWidth = ImGui::CalcTextSize(m_DisplayPath.c_str());
                const float s = windowSize.x - pathWidth.x;
                windowSize.x -= s;
            }
            m_DisplayPath.pop_back();
            m_DisplayPath.pop_back();
            m_DisplayPath.pop_back();
            m_DisplayPath += "...";
        }
        const ImGuiIO& io = ImGui::GetIO();

        ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowBgAlpha(1);
        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos({ (io.DisplaySize.x - windowSize.x) / 2.f, (io.DisplaySize.y - windowSize.y) / 2.f });
        ImGui::Begin("Image writer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::Image((void*)(intptr_t)m_Image.GetGpuImage(), imageSize);
        ImGui::SameLine();
        ImVec2 currsorPos = ImGui::GetCursorPos();
        ImGui::Text("%s", m_DisplayPath.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Path", PathButtonSize))
            Thread::Dispatch(SaveFileDialog);
        ImGui::SetCursorPos({ currsorPos.x, currsorPos.y + ImGui::GetItemRectSize().y + 5 });
        const float btnWidth = (pathWidth.x + PathButtonSize.x) / 2.f;
        if (ImGui::Button("Save", { btnWidth, 0 }))
        {
            Thread::Dispatch(SaveImageToFile);
            ImGui::End();
            return true;
        }
        ImGui::SameLine();
        const bool close = ImGui::Button("Cancel", { btnWidth, 0 });
        if (close)
            Reset();
        ImGui::Checkbox("Upscale image", &m_UpscaleOnWrite);
        ImGui::End();
        return close;
    }

    static inline void Reset()
    {
        m_Image.Reset();
    }
};