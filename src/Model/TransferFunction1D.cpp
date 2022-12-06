#include "TransferFunction1D.h"

void TransferFunction1D::AddOpacityStop(R32 intensity, R32 opacity)
{
    m_omap.m_intensities.push_back(intensity);
    m_omap.m_opacities.push_back(opacity);
}

void TransferFunction1D::AddColorStop(R32 intensity, R32 red, R32 green, R32 blue)
{
    m_cmap.m_intensities.push_back(intensity);
    m_cmap.m_rgb.push_back({ red, green, blue });
}

GLuint TransferFunction1D::CreateTransferFunctionTexture()
{
    auto image = CreateImageFromData();
    glEnable   (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!m_isValid)
        glGenTextures(1, &m_cmapTex);   
    
    glBindTexture(GL_TEXTURE_2D, m_cmapTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, (U8*) image.data());
    glBindTexture(GL_TEXTURE_2D, 0);
    m_isValid = true;

    return m_cmapTex;
}

std::array<U8, 4 * 100> TransferFunction1D::CreateImageFromData()
{
    U32 leftColorEnd  = 0, leftOpacityEnd  = 0;
    U32 rightColorEnd = 1, rightOpacityEnd = 1;

    std::array<U8, 4 * 100> textureData = {};
    for (U32 i = 0; i < 100; ++i)
    {
        R32 t  = (R32) i / 100.0f;
        R32 colRightIntensity     = m_cmap.m_intensities[rightColorEnd];
        R32 opacityRightIntensity = m_omap.m_intensities[rightOpacityEnd];

        if (t > colRightIntensity)
        {
            leftColorEnd  = std::min((U32) m_cmap.m_intensities.size() - 1, leftColorEnd + 1);
            rightColorEnd = std::min((U32) m_cmap.m_intensities.size() - 1, rightColorEnd + 1);
        }

        if (t > opacityRightIntensity)
        {
            leftOpacityEnd  = std::min((U32) m_omap.m_intensities.size() - 1, leftOpacityEnd + 1);
            rightOpacityEnd = std::min((U32) m_omap.m_intensities.size() - 1, rightOpacityEnd + 1);
        }

        colRightIntensity     = m_cmap.m_intensities[rightColorEnd];
        opacityRightIntensity = m_omap.m_intensities[rightOpacityEnd];
        R32 colLeftIntensity     = m_cmap.m_intensities[leftColorEnd];
        R32 opacityLeftIntensity = m_omap.m_intensities[leftOpacityEnd];

        R32 cL_r = m_cmap.m_rgb[leftColorEnd][0];
        R32 cL_g = m_cmap.m_rgb[leftColorEnd][1];
        R32 cL_b = m_cmap.m_rgb[leftColorEnd][2];

        R32 cR_r = m_cmap.m_rgb[rightColorEnd][0];
        R32 cR_g = m_cmap.m_rgb[rightColorEnd][1];
        R32 cR_b = m_cmap.m_rgb[rightColorEnd][2];

        R32 opacityLeft  = m_omap.m_opacities[leftOpacityEnd];
        R32 opacityRight = m_omap.m_opacities[rightOpacityEnd];

        float tWithinRGB = (t - colLeftIntensity) / (colRightIntensity - colLeftIntensity);
        float tWithinA   = (t - opacityLeftIntensity) / (opacityRightIntensity - opacityLeftIntensity);

        U8 r = (U8)(lerp(cL_r, cR_r, tWithinRGB) * 255.0f);
        U8 g = (U8)(lerp(cL_g, cR_g, tWithinRGB) * 255.0f);
        U8 b = (U8)(lerp(cL_b, cR_b, tWithinRGB) * 255.0f);
        U8 a = (U8)(lerp(opacityLeft, opacityRight, tWithinA) * 255.0f);

        textureData[i * 4 + 0] = r;
        textureData[i * 4 + 1] = g;
        textureData[i * 4 + 2] = b;
        textureData[i * 4 + 3] = a;
    }

    return textureData;
}

