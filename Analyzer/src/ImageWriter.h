#pragma once
#include <filesystem>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "ImGui/imgui.h"
#include "nfd/nfd.h"

#include "Window.h"
#include "Image.h"

class ImageWriter
{
private:
    static constexpr ImVec2 PathButtonSize{ 100, 0 };
    static constexpr float PathButtonPadding = 20.f;
    static inline Image m_Image;
    static inline std::string m_Path = std::filesystem::current_path().string() + "\\plot.png";
private:
    static inline void SaveFileDialog()
    {
        nfdchar_t* savePath = NULL;
        nfdresult_t result = NFD_SaveDialog("png;jpg;jpeg", NULL, &savePath);
        if (result == NFD_OKAY)
        {
            m_Path = savePath;
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
    }
public:
    static inline bool SaveImage(ImVec2 size, ImVec2 pos)
    {
        m_Image.Create(size, pos);
        const float aspecRatio = size.x / size.y;
        const ImVec2 imageSize{ 500 * aspecRatio, 500 };
        const ImVec2 pathWidth = ImGui::CalcTextSize(m_Path.c_str());
        const ImVec2 windowSize{ imageSize.x + pathWidth.x + PathButtonSize.x + PathButtonPadding, imageSize.y + 25 /*Title height*/};
        const ImGuiIO& io = ImGui::GetIO();

        ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowBgAlpha(1);
        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos({ (io.DisplaySize.x - windowSize.x)/2.f, (io.DisplaySize.y-windowSize.y)/2.f });
        ImGui::Begin("Image writer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::Image((void*)(intptr_t)m_Image.GetGpuImage(), imageSize);
        ImGui::SameLine();
        ImVec2 currsorPos = ImGui::GetCursorPos();
        ImGui::Text("%s", m_Path.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Path", PathButtonSize))
            SaveFileDialog();
        ImGui::SetCursorPos({ currsorPos.x, currsorPos.y + ImGui::GetItemRectSize().y + 5});
        if (ImGui::Button("Save", { pathWidth.x + PathButtonSize.x, 0 }))
            SaveImageToFile();
        ImGui::End();
        return false;
    }
};