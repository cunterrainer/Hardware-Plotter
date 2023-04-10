#pragma once
#include <mutex>
#include <algorithm>
#include <filesystem>

#include "stb/stb_image_write.h"
#include "ImGui/imgui.h"
#include "nfd/nfd.h"

#include "Log.h"
#include "Image.h"
#include "RenderWindow.h"

#ifdef WINDOWS
    #define DEFAULT_PATH "\\plot.png"
#elif defined(LINUX)
    #define DEFAULT_PATH "/plot.png"
#endif

class ImageWriter
{
private:
    static constexpr ImVec2 PathButtonSize{ 100, 0 };
    static constexpr float  SettingsSectionWidth = 350;
private:
    Image* m_Image;
    std::string m_Path = std::filesystem::current_path().string() + DEFAULT_PATH;
    std::string m_DisplayPath = m_Path;
    mutable std::mutex m_Mutex;
    bool m_Open = false;

    ImVec2 m_ImageSize;
    ImVec2 m_WindowSize;
    ImVec2 m_ItemSpacing;
    float  m_BtnWidth = 0;
    float  m_AspectRatio = 0;

    bool m_UpscaleOnWrite = false;
    bool m_KeepAspectRatio = true;
    int  m_NewWidth = 0;
    int  m_NewHeight = 0;
private:
    inline void SaveFileDialog()
    {
        nfdchar_t* savePath = NULL;
        nfdresult_t result = NFD_SaveDialog("png;jpg;jpeg", NULL, &savePath);
        if (result == NFD_OKAY)
        {
            std::lock_guard lock(m_Mutex);
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

    inline void WriteImageDisplayError() const
    {
        const std::string errorMsg = "Failed to write to file: " + m_Path + " w: " + std::to_string(m_Image->Width()) + " h: " + std::to_string(m_Image->Height()) + " c: " + std::to_string(Image::NumOfChannels);
        Err << errorMsg << Endl;
        MsgBoxError(errorMsg.c_str());
    }

    inline void WriteImage(int(*write_func)(const char*, int, int, int, const void*, int)) const
    {
        std::lock_guard lock(m_Mutex);
        if (!write_func(m_Path.c_str(), m_Image->Width(), m_Image->Height(), Image::NumOfChannels, m_Image->Data(), m_Image->Width() * Image::NumOfChannels))
            WriteImageDisplayError();
        else
            Log << "Successfully wrote image to file '" << m_Path.c_str() << "' w: " << m_Image->Width() << " h: " << m_Image->Height() << " c: " << Image::NumOfChannels << Endl;
    }

    inline void WriteImage(int(*write_func)(const char*, int, int, int, const void*)) const
    {
        std::lock_guard lock(m_Mutex);
        if (!write_func(m_Path.c_str(), m_Image->Width(), m_Image->Height(), Image::NumOfChannels, m_Image->Data()))
            WriteImageDisplayError();
        else
            Log << "Successfully wrote image to file '" << m_Path.c_str() << "' w: " << m_Image->Width() << " h: " << m_Image->Height() << " c: " << Image::NumOfChannels << Endl;
    }

    inline void SaveImageToFile()
    {
        std::filesystem::path path;
        {
            std::lock_guard lock(m_Mutex);
            path = m_Path;
        }
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });

        if (m_UpscaleOnWrite)
        {
            int w = 0;
            int h = 0;
            {
                std::lock_guard lock(m_Mutex);
                w = m_NewWidth;
                h = m_NewHeight;
            }
            const std::string errorMsg = m_Image->ScaleUp(w, h); // No other function changes values of image
            if (!errorMsg.empty())
            {
                Err << errorMsg << Endl;
                MsgBoxError(errorMsg.c_str());
            }
            else
                Log << "Successfully resized the image w: " << w << " h: " << h << " c: " << Image::NumOfChannels << Endl;
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
            {
                std::lock_guard lock(m_Mutex);
                m_Path += ".png";
            }
            WriteImage(stbi_write_png);
        }
    }

    inline void CalcProperties()
    {
        {
            std::lock_guard lock(m_Mutex);
            if (m_NewHeight == 0 && m_NewWidth == 0)
            {
                m_NewHeight = m_Image->Height();
                m_NewWidth = m_Image->Width();
            }
        }

        const ImVec2 size = ImGui::GetWindowSize();
        m_AspectRatio = (float)m_Image->Width() / (float)m_Image->Height();
        m_ImageSize = { (size.y-25) * m_AspectRatio, size.y - 25 /*Title height*/ };
        m_WindowSize = { m_ImageSize.x + SettingsSectionWidth, size.y };

        ImVec2 pathWidth = ImGui::CalcTextSize(m_DisplayPath.c_str());
        if (pathWidth.x > PathButtonSize.x - m_ItemSpacing.x * 2)
        {
            std::lock_guard lock(m_Mutex);
            while (m_DisplayPath.size() > 4 && pathWidth.x > SettingsSectionWidth - PathButtonSize.x - m_ItemSpacing.x * 2)
            {
                m_DisplayPath.pop_back();
                pathWidth = ImGui::CalcTextSize(m_DisplayPath.c_str());
            }
            m_DisplayPath.pop_back();
            m_DisplayPath.pop_back();
            m_DisplayPath.pop_back();
            m_DisplayPath += "...";
        }
    }

    inline void NextLine() const
    {
        ImGui::NewLine();
        ImGui::SameLine(m_ImageSize.x + m_ItemSpacing.x);
    }

    inline void Upscalar()
    {
        ImGui::SetNextItemWidth(m_BtnWidth);
        ImGui::Checkbox("Upscale image", &m_UpscaleOnWrite);
        if (m_UpscaleOnWrite)
        {
            ImGui::SameLine(m_WindowSize.x - m_BtnWidth - m_ItemSpacing.x);
            if (ImGui::Checkbox("Keep aspect ratio", &m_KeepAspectRatio) && m_KeepAspectRatio)
            {
                std::lock_guard lock(m_Mutex);
                m_NewHeight = m_Image->Height();
                m_NewWidth = m_Image->Width();
            }
            NextLine();
            ImGui::SetNextItemWidth(m_BtnWidth);
            if (ImGui::InputInt("##WidthInput", &m_NewWidth) && m_KeepAspectRatio)
            {
                std::lock_guard lock(m_Mutex);
                m_NewWidth = std::max(m_NewWidth, 1);
                m_NewHeight = static_cast<int>((float)m_NewWidth / m_AspectRatio);
                m_NewHeight = std::max(m_NewHeight, 1);
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(m_BtnWidth);
            if (ImGui::InputInt("##HeightInput", &m_NewHeight))
            {
                std::lock_guard lock(m_Mutex);
                m_NewHeight = std::max(m_NewHeight, 1);
                m_NewWidth = static_cast<int>((float)m_NewHeight * m_AspectRatio);
                m_NewWidth = std::max(m_NewWidth, 1);
            }
        }
    }
public:
    inline explicit ImageWriter(Image* img) : m_Image(img) {}

    inline void Close()
    {
        std::lock_guard lock(m_Mutex);
        m_NewWidth = 0;
        m_NewHeight = 0;
        m_KeepAspectRatio = true;
        m_UpscaleOnWrite = false;
        m_Open = false;
    }

    inline bool IsOpen() const
    {
        return m_Open;
    }

    bool SaveImage();
};