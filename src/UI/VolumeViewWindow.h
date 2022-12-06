#pragma once

#include "../Graphics/Texture.h"
#include "../Graphics/Shader.h"
#include "../Model/VolumeModel.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "../IO/MouseButtonState.h"

namespace vr
{
    enum class ProjectionMethod
    {
        MAX_INTENSITY = 0,
        AVG_INTENSITY = 1,
        FIRST_HIT = 2,
        COMPOSITE = 3
    };

    class VolumeViewWindow
    {
        public:
            VolumeViewWindow() = default;
            
            void Initialize();
            void RenderUI(GLFWwindow* window);     
            void UpdateVolumeModel(VolumeModel* newModel);
            void ProcessInput(GLFWwindow* window);
            void Refresh();
            void UpdateProjectionMethod(ProjectionMethod newMethod, const float firstHitThreshold = 1.0f);
            ~VolumeViewWindow();
        private:
            void UpdateScreenFrame(uint32_t width, uint32_t height);
            
            uint32_t m_prevWidth  = 1;
            uint32_t m_prevHeight = 1;

            GLuint m_screenColor {0};
            GLuint m_framebuffer {0};
            GLuint m_screenRbo   {0};

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

            Shader m_volumeShaderSimple;
            Shader m_volumeShaderComposite;

            VolumeModel* m_volumeModel = nullptr;

            bool m_isCacheValid = false;
            
    };
}
