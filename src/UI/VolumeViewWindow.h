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
            
            void UpdateVolumeModel(VolumeModel* newModel);

            virtual void Initialize() = 0;
            virtual void RenderUI(GLFWwindow* window) = 0;

            void ProcessInput(GLFWwindow* window);
            void Refresh();
            void UpdateProjectionMethod(ProjectionMethod newMethod, const float firstHitThreshold = 1.0f);

            virtual ~VolumeViewWindow() {};

        protected:
            void UpdateScreenFrame(uint32_t width, uint32_t height);
            

            /**
             *  The model of the volume itself is simply stored as a raw pointer given that
             *  no ownership responsibilities are placed on the views themselves. 
             **/
            VolumeModel* m_volumeModel = nullptr;

            /**
             *  Identifiers for the OpenGL buffers that are used for the framebuffer,
             *  framebuffers are used in this project to render to multiple windows but also 
             *  have a way of managing the state of the buffer such that the 3D/2D views are only
             *  re-rendered when necessary, i.e. whenever m_isCacheValid == false
             **/
            GLuint m_screenColor {0};
            GLuint m_framebuffer {0};
            GLuint m_screenRbo   {0};

            uint32_t m_prevWidth  = 1;
            uint32_t m_prevHeight = 1;

            //The current framebuffer is 'invalidated' whenever this is false
            bool m_isCacheValid = false;
            
    };
}
