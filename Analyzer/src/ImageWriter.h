#pragma once
#include <algorithm>
#include <filesystem>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "ImGui/imgui.h"
#include "nfd/nfd.h"

#include "Window.h"
#include "Thread.h"
#include "Image.h"
#include "Log.h"

class ImageWriter
{
private:
    static constexpr ImVec2 PathButtonSize{ 100, 0 };
    static constexpr float PathButtonPadding = 20.f;
private:
    static inline Image* m_Image;
    static inline std::string m_Path = std::filesystem::current_path().string() + "\\plot.png";
    static inline std::string m_DisplayPath = m_Path;
    static inline bool m_Open = false;

    static inline ImVec2 m_ImageSize;
    static inline ImVec2 m_PathWidth;
    static inline ImVec2 m_WindowSize;
    static inline float  m_BtnWidth;
    static inline float  m_AspectRatio;

    static inline bool m_UpscaleOnWrite = false;
    static inline bool m_KeepAspectRatio = true;
    static inline int  m_NewWidth = 0;
    static inline int  m_NewHeight = 0;
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
            Log << "[NFD] Selected file: " << m_Path << Endl;
        }
        else if (result == NFD_CANCEL)
            Log << "[NFD] User pressed cancel" << Endl;
        else // error opening the file
        {
            Err << "[NFD] Failed to open the dialog: " << NFD_GetError() << Endl;
            MsgBoxError(std::string("Failed to open file dialog: " + std::string(NFD_GetError())).c_str());
        }
    }

    static inline void WriteImageDisplayError()
    {
        const std::string errorMsg = "Failed to write to file: " + m_Path + " w: " + std::to_string(m_Image->Width()) + " h: " + std::to_string(m_Image->Height()) + " c: " + std::to_string(Image::NumOfChannels);
        Err << errorMsg << Endl;
        MsgBoxError(errorMsg.c_str());
    }

    static inline void WriteImage(int(*write_func)(const char*, int, int, int, const void*, int))
    {
        if (!write_func(m_Path.c_str(), m_Image->Width(), m_Image->Height(), Image::NumOfChannels, m_Image->Data(), m_Image->Width() * Image::NumOfChannels))
            WriteImageDisplayError();
        else
            Log << "Successfully wrote image to file '" << m_Path.c_str() << "' w: " << m_Image->Width() << " h: " << m_Image->Height() << " c: " << Image::NumOfChannels << Endl;
    }

    static inline void WriteImage(int(*write_func)(const char*, int, int, int, const void*))
    {
        if (!write_func(m_Path.c_str(), m_Image->Width(), m_Image->Height(), Image::NumOfChannels, m_Image->Data()))
            WriteImageDisplayError();
        else
            Log << "Successfully wrote image to file '" << m_Path.c_str() << "' w: " << m_Image->Width() << " h: " << m_Image->Height() << " c: " << Image::NumOfChannels << Endl;
    }

    static inline void SaveImageToFile()
    {
        std::filesystem::path path(m_Path);
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });

        if (m_UpscaleOnWrite)
        {
            const std::string errorMsg = m_Image->ScaleUp(m_NewWidth, m_NewHeight);
            if (!errorMsg.empty())
            {
                Err << errorMsg << Endl;
                MsgBoxError(errorMsg.c_str());
            }
            else
                Log << "Successfully resized the image w: " << m_NewWidth << " h: " << m_NewHeight << " c: " << Image::NumOfChannels << Endl;
        }
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

    static inline void CalcWindowProps(ImVec2 wSize)
    {
        if (wSize.x <= 0 || wSize.y <= 0)
            return;
        if (m_NewHeight == 0 && m_NewWidth == 0)
        {
            m_NewHeight = m_Image->Height();
            m_NewWidth = m_Image->Width();
        }
        m_AspectRatio = wSize.x / wSize.y;
        m_ImageSize = { wSize.y / 1.7f * m_AspectRatio, wSize.y / 1.7f };
        m_PathWidth = ImGui::CalcTextSize(m_DisplayPath.c_str());
        m_PathWidth.x = std::max(m_PathWidth.x, 300.f);
        m_WindowSize = { m_ImageSize.x + m_PathWidth.x + PathButtonSize.x + PathButtonPadding, m_ImageSize.y + 25 /*Title height*/ };
        if (m_WindowSize.x >= wSize.x)
        {
            while (m_DisplayPath.size() > 4 && m_WindowSize.x >= wSize.x)
            {
                m_DisplayPath.pop_back();
                m_PathWidth = ImGui::CalcTextSize(m_DisplayPath.c_str());
                const float s = m_WindowSize.x - m_PathWidth.x;
                m_WindowSize.x -= s;
            }
            m_DisplayPath.pop_back();
            m_DisplayPath.pop_back();
            m_DisplayPath.pop_back();
            m_DisplayPath += "...";
        }
    }

    static inline void SetWindowProps()
    {
        const ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowBgAlpha(1);
        ImGui::SetNextWindowSize(m_WindowSize);
        ImGui::SetNextWindowPos({ (io.DisplaySize.x - m_WindowSize.x) / 2.f, (io.DisplaySize.y - m_WindowSize.y) / 2.f });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    }

    static inline void NextLine()
    {
        ImGui::NewLine();
        ImGui::SameLine(m_ImageSize.x + 7);
    }

    static inline void Upscalar()
    {
        ImGui::SetNextItemWidth(m_BtnWidth);
        ImGui::Checkbox("Upscale image", &m_UpscaleOnWrite);
        if (m_UpscaleOnWrite)
        {
            ImGui::SameLine(m_WindowSize.x - m_BtnWidth - 4);
            if (ImGui::Checkbox("Keep aspect ratio", &m_KeepAspectRatio) && m_KeepAspectRatio)
            {
                m_NewHeight = m_Image->Height();
                m_NewWidth = m_Image->Width();
            }
            NextLine();
            ImGui::SetNextItemWidth(m_BtnWidth);
            if (ImGui::InputInt("##WidthInput", &m_NewWidth) && m_KeepAspectRatio)
            {
                m_NewWidth = std::max(m_NewWidth, 1);
                m_NewHeight = static_cast<int>((float)m_NewWidth / m_AspectRatio);
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(m_BtnWidth);
            if (ImGui::InputInt("##HeightInput", &m_NewHeight))
            {
                m_NewHeight = std::max(m_NewHeight, 1);
                m_NewWidth = static_cast<int>((float)m_NewHeight * m_AspectRatio);
            }
        }
    }
public:
    static inline void Reset()
    {
        m_NewWidth = 0;
        m_NewHeight = 0;
        m_KeepAspectRatio = true;
        m_UpscaleOnWrite = false;
        m_Open = false;
    }

    static inline bool IsOpen()
    {
        return m_Open;
    }

    static inline bool SaveImage(ImVec2 windowSize, Image* img)
    {
        m_Image = img;
        m_Open = true;
        CalcWindowProps(windowSize);
        SetWindowProps();

        ImGui::Begin("Image writer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::Image((void*)(intptr_t)m_Image->GetGpuImage(), m_ImageSize);
        ImGui::SameLine();
        ImVec2 currsorPos = ImGui::GetCursorPos();
        ImGui::Text("%s", m_DisplayPath.c_str());
        ImGui::SameLine(m_WindowSize.x - PathButtonSize.x - 3);
        if (ImGui::Button("Path", PathButtonSize))
            Thread::Dispatch(SaveFileDialog);
        ImGui::SetCursorPos({ currsorPos.x, currsorPos.y + ImGui::GetItemRectSize().y + 5 });
        m_BtnWidth = (m_PathWidth.x + PathButtonSize.x) / 2.f;
        if (ImGui::Button("Save", { m_BtnWidth, 0 }))
            Thread::Dispatch(SaveImageToFile);
        ImGui::SameLine();
        const bool close = ImGui::Button("Cancel", { m_BtnWidth, 0 }) || img->Width() <= 0 || img->Height() <= 0;
        NextLine();
        Upscalar();
        ImGui::End();
        ImGui::PopStyleVar();
        return close;
    }
};