#pragma once
#include <string>
#include <functional>
#include <string_view>
#include <unordered_map>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#include "Image.h"
#include "Plot.h"

class PlotManager
{
private:
    std::unordered_map<std::string, Plot<double>> m_Plots;
    float m_YOffset;
    Image m_Image;
    bool m_SavingImage = false;
private:
    inline void CreateImage(ImVec2 windowSize) 
    {
        std::vector<std::reference_wrapper<const Image>> images;
        size_t count = 0;
        size_t pixels = 0;
        size_t height = 0;
        size_t width = 0;
        float windowHeight = (windowSize.y - m_YOffset) / (float)m_Plots.size();
        for (auto it = m_Plots.begin(); it != m_Plots.end(); ++it)
        {
            const Image& img = it->second.Capture({ windowSize.x, windowHeight }, m_YOffset + windowHeight * (float)count);
            pixels += img.Height() * img.Width() * Image::NumOfChannels;
            height += img.Height();
            width = img.Width();
            images.push_back(std::cref(img));
            ++count;
        }

        size_t offset = 0;
        unsigned char* pixel = new unsigned char[pixels];
        for (const Image& img : images)
        {
            const size_t size = img.Height() * img.Width() * Image::NumOfChannels;
            std::memcpy(&pixel[offset], img.Data(), size);
            offset += size;
        }
        m_Image.Assign(pixel, width, height);
        m_Image.CreateGpuImage();
    }
public:
    inline explicit PlotManager(float yOffset) : m_YOffset(yOffset) {}

    inline void Add(const std::string& plotName, std::string_view ylabel, const std::string& graphName, double x, double y)
    {
        m_Plots[plotName].Add(graphName, ylabel, x, y);
    }

    inline bool SaveAllPlots(ImVec2 windowSize)
    {
        if (!m_Image.Created())
            CreateImage(windowSize);
        windowSize = { windowSize.x, windowSize.y - m_YOffset };
        if (m_SavingImage || !ImageWriter::IsOpen())
        {
            m_SavingImage = true;
            if (ImageWriter::SaveImage({windowSize.x, windowSize.y-29}, &m_Image))
            {
                m_SavingImage = false;
                m_Image.Reset();
                for (auto it = m_Plots.begin(); it != m_Plots.end(); ++it)
                    it->second.ResetImage();
                ImageWriter::Reset();
                return true;
            }
        }
        return false;
    }

    inline void Render(ImVec2 windowSize, bool debugInfo)
    {
        size_t count = 0;
        float height = (windowSize.y - m_YOffset) / (float)m_Plots.size();
        for (auto it = m_Plots.begin(); it != m_Plots.end(); ++it)
        {
            it->second.Render({ windowSize.x, height }, m_YOffset + height*(float)count, it->first.c_str(), debugInfo);
            ++count;
        }
    }

    inline void Delete()
    {
        m_Plots.clear();
    }

    inline void CleanupGraphs()
    {
        for (auto it = m_Plots.begin(); it != m_Plots.end(); ++it)
        {
            it->second.CleanupGraphs();
        }
    }
};