#pragma once
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"
#include "GLFW/glfw3.h"

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

    inline void FlipVertically()
    {
        for (int w = 0; w < m_Width; ++w)
        {
            for (int h = 0; h < m_Height / 2; ++h)
            {
                const int idx = (m_Width * h + w) * NumOfChannels;
                const int EndIdx = m_Width*m_Height* NumOfChannels - m_Width * NumOfChannels * (h + 1) + w * NumOfChannels;
                for (int c = 0; c < NumOfChannels; ++c)
                {
                    GLubyte temp = m_Pixel[idx + c];
                    m_Pixel[idx + c] = m_Pixel[EndIdx + c];
                    m_Pixel[EndIdx + c] = temp;
                }
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

    inline void ScaleUp(int newWidth, int newHeight)
    {
        GLubyte* pixelScaled = new GLubyte[(unsigned int)(NumOfChannels * newWidth * newHeight)];
        if (!stbir_resize_uint8(m_Pixel, m_Width, m_Height, m_Width * NumOfChannels, pixelScaled, newWidth, newHeight, newWidth * NumOfChannels, NumOfChannels))
        {
            Err << "Failed to resize image w_old: " << m_Width << " h_old: " << m_Height << " w_new: " << newWidth << " h_new: " << newHeight << " channel: " << NumOfChannels << Endl;
            return;
        }
        m_Width = newWidth;
        m_Height = newHeight;
        delete[] m_Pixel;
        m_Pixel = pixelScaled;
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