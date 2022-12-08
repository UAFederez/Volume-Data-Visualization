#pragma once

#include "VolumeViewWindow.h"

namespace vr
{
    class VolumeViewWindow3D final : public VolumeViewWindow
    {
        public:

            virtual void Initialize() override;
            virtual void RenderUI(GLFWwindow* window) override;
            void UpdateProjectionMethod(ProjectionMethod newMethod, const float firstHitThreshold = 1.0f);

            ~VolumeViewWindow3D();

        private:

            GLuint m_volumeVao {0};
            GLuint m_volumeVbo {0};
            GLuint m_volumeEbo {0};

            ProjectionMethod m_projMethod = ProjectionMethod::MAX_INTENSITY;
            float m_firstHitThreshold     = 0.0f;

            MouseButtonState m_leftButtonState { GLFW_MOUSE_BUTTON_LEFT };

            glm::mat4 m_modelScaleMat = glm::mat4(1.0f);
            glm::mat4 m_projectionMat = glm::mat4(1.0f);
            glm::mat4 m_viewMat       = glm::mat4(1.0f);
            glm::mat4 m_rotationMat   = glm::mat4(1.0f);
            glm::vec3 m_cameraPos     = glm::vec3(0.0f);

            glm::vec3 m_mouseHemi0    = glm::vec3(0.0f);
            glm::vec3 m_mouseHemi1    = glm::vec3(0.0f);
            glm::vec3 m_rotateAxis    = glm::vec3(0.0f);
            float     m_rotateAngle   = 0.0f;
            glm::mat4 m_currRotation  = glm::mat4(1.0f);
            glm::mat4 m_prevRotation  = glm::mat4(1.0f);

            glm::vec2 m_prevDragVec   = glm::vec2(1.0f);
            R32 m_distAlongZoomAxis     = 0.0f;
            R32 m_prevDistAlongZoomAxis = m_distAlongZoomAxis;

            // Shader responsible for drawing the outline box around the 3d visualization
            Shader m_volumeBoundsShader;
            
            // Shader responsible for MIP, AIP and first-hit raycasting
            Shader m_volumeShaderSimple;

            // Shader responsible for composite shading
            Shader m_volumeShaderComposite;


    };
}
