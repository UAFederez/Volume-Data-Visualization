#pragma once

#include "VolumeViewWindow.h"
#include "../IO/MouseButtonState.h"
#include "../Graphics/Shader.h"

#include <GLFW/glfw3.h>

namespace vr
{
    enum class AnatomicalAxis {
        CORONAL,
        SAGITTAL,
        HORIZONTAL
    };

    class VolumeViewWindow2D : public VolumeViewWindow
    {
        public:
            VolumeViewWindow2D(const std::string& windowName, AnatomicalAxis axis) :
                VolumeViewWindow(), 
                m_name(windowName),
                m_axis(axis)
            {

            }

            virtual void Initialize() override;
            virtual void RenderUI(GLFWwindow* window) override;
        private:
            // OpenGL buffers to store data about the curent image to be rendered
            // and its geometry
            GLuint m_volumeImageVao {0};
            GLuint m_volumeImageVbo {0};
            GLuint m_volumeImageEbo {0};

            // Shaders to render a quad to the screen
            Shader m_imageQuadShader;

            // Stores the orthographic projection matrix for the 2D view
            glm::mat4 m_projectionmat = glm::mat4(1.0f);

            // Stores the transformation corresponding to camera zoom
            glm::mat4 m_modelScaleMat = glm::mat4(1.0f);

            // Stores the transformation corresponding to camera panning
            glm::mat4 m_viewMat = glm::mat4(1.0f);

            MouseButtonState m_leftButtonState = { GLFW_MOUSE_BUTTON_LEFT };

            // Text displayed in the titlebar of the panel
            const std::string m_name;

            // Which anatomical axis is this panel viewing from
            AnatomicalAxis m_axis   = AnatomicalAxis::CORONAL;

            // Index of the currently viewed slice along this axis
            I32 m_currSliceIndex = 0;
            I32 m_prevSliceIndex = 0;

            // Maximum number of slices along this axis
            I32 m_sliceExtent = 0;    

            /**
             * Variables for camera controls
            **/
            R32 m_zoomLevel = 100.0f;
            glm::mat4 m_currCameraPan = glm::mat4(1.0f);
            glm::mat4 m_prevCameraPan = glm::mat4(1.0f);
    };
}

