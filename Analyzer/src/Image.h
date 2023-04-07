#pragma once
#include <string_view> // std::swap

#include "GLFW/glfw3.h"

#include "Log.h"

class Image
{
public:
    static constexpr int NumOfChannels = 4;
private:
    GLubyte* m_Pixel = nullptr;
    GLsizei m_Width = 0;
    GLsizei m_Height = 0;
    GLuint m_GpuImage = 0;
    bool m_Created = false;
private:
    inline void Delete()
    {
        if (m_GpuImage == 0 || m_Pixel == nullptr)
            return;

        delete[] m_Pixel;
        glDeleteTextures(1, &m_GpuImage);
        m_Width = 0;
        m_Height = 0;
        m_GpuImage = 0;
        m_Created = false;
    }

    inline void FlipHorizontally()
    {
        for (int row = 0; row < m_Height; row++)
        {
            for (int col = 0; col < m_Width / 2; col++)
            {
                for (int channel = 0; channel < NumOfChannels; channel++)
                    std::swap(m_Pixel[(row * m_Width + col) * NumOfChannels + channel], m_Pixel[(row * m_Width + m_Width - 1 - col) * NumOfChannels + channel]);
            }
            if (m_Width % 2 != 0) // handle center column for odd number of columns
            {
                int center_col = m_Width / 2;
                for (int channel = 0; channel < NumOfChannels; channel++)
                    std::swap(m_Pixel[(row * m_Width + center_col) * NumOfChannels + channel], m_Pixel[(row * m_Width + center_col) * NumOfChannels + channel]);
            }
        }
    }

    inline void FlipVertically()
    {
        for (int row = 0; row < m_Height / 2; row++)
        {
            for (int col = 0; col < m_Width; col++)
            {
                for (int channel = 0; channel < NumOfChannels; channel++)
                    std::swap(m_Pixel[(row * m_Width + col) * NumOfChannels + channel], m_Pixel[((m_Height - 1 - row) * m_Width + col) * NumOfChannels + channel]);
            }
        }

        if (m_Height % 2 == 1)
        {
            int midRow = m_Height / 2;
            for (int col = 0; col < m_Width / 2; col++)
            {
                for (int channel = 0; channel < NumOfChannels; channel++)
                    std::swap(m_Pixel[(midRow * m_Width + col) * NumOfChannels + channel], m_Pixel[(midRow * m_Width + m_Width - 1 - col) * NumOfChannels + channel]);
            }
        }
    }
public:
    inline ~Image()
    {
        Delete();
    }

    inline void Reset()
    {
        Delete();
    }

    inline void Create(ImVec2 size, ImVec2 pos)
    {
        if (m_Created) return;
        m_Created = true;
        m_Width = (GLsizei)size.x;
        m_Height = (GLsizei)size.y;
        m_Pixel = new GLubyte[(unsigned int)(NumOfChannels * m_Width * m_Height)];

        // Create a OpenGL texture identifier
        glGenTextures(1, &m_GpuImage);
        glBindTexture(GL_TEXTURE_2D, m_GpuImage);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glReadPixels((GLint)pos.x, (GLint)pos.y, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, m_Pixel);
        FlipVertically();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Pixel);
    }

    inline GLuint GetGpuImage() const
    {
        return m_GpuImage;
    }

    inline int Height() const { return m_Height; }
    inline int Width() const { return m_Width; }
    inline unsigned char* Data() const { return m_Pixel; }
};