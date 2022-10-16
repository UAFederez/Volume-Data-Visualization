#pragma once

#include <GL/glew.h>
#include <wx/glcanvas.h>
#include <vector>

#include "VolumeDataset.h"

struct ColorMap
{
    std::vector<R32> m_intensities;
    std::vector<std::array<R32, 3>> m_rgb;         // low 3 bytes are used as rgb values
};

struct OpacityMap
{
    std::vector<R32> m_intensities;
    std::vector<R32> m_opacities;
};

template <typename T>
T lerp(const T& a, const T& b, const R32 t)
{
    return (T)((1 - t) * (R32) a + t * (R32) b);
}

struct TransferFunction1D
{
    public:
        TransferFunction1D() = default;

        void AddOpacityStop(R32 intensity, R32 opacity);
        void AddColorStop(R32 intensity, R32 red, R32 green, R32 blue);
        std::array<U8, 4 * 100> CreateImageFromData();

        ColorMap   m_cmap    = { };
        OpacityMap m_omap    = { };
        GLuint     m_cmapTex = {0};
        GLuint     m_omapTex = {0};
};

