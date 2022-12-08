#include "VolumeViewWindow.h"
#include <vector>
#include <cstdint>

#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace vr
{
    void VolumeViewWindow::Refresh()
    {
        m_isCacheValid = false;
    }

    void VolumeViewWindow::UpdateVolumeModel(VolumeModel* newModel)
    {
        m_volumeModel = newModel;
    }

    void VolumeViewWindow::ProcessInput(GLFWwindow* window)
    {
        
    }

    void VolumeViewWindow::UpdateScreenFrame(uint32_t width, uint32_t height)
    {
        glBindTexture(GL_TEXTURE_2D, m_screenColor);
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glBindRenderbuffer(GL_RENDERBUFFER, m_screenRbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        m_prevWidth    = width;
        m_prevHeight   = height;
        m_isCacheValid = true;

    }
}
