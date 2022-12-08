#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

#include "Model/VolumeDataset.h"
#include "Model/VolumeModel.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "IO/MouseButtonState.h"
#include "UI/VolumeViewWindow3D.h"
#include "AsyncTask.h"

namespace vr
{
    class MainApplication
    {
        public:
            MainApplication();
            MainApplication(const MainApplication&)            = delete;
            MainApplication& operator=(const MainApplication&) = delete;

            void RunMainLoop();

            ~MainApplication();
        private:
            void ProcessMouseClick();
            void ShowMetadataModal();
            void AsyncLoadDataset();

            void Init();
            
            std::string m_volumeFilePath = "";
            std::array<U32, 3> m_inputDimensions = { 0 , 0 , 0 , };
            std::array<R64, 3> m_inputSpacings   = { 1., 1., 1., };
            int m_inputTypeIndex = 0;

            Shader m_raycastShader;

            VolumeViewWindow3D m_viewport3d;

            std::unique_ptr<VolumeModel> m_volumeModel = {};
            AsyncTask<std::optional<VolumeDataset>> m_loadDatasetTask;
            
            // Mouse input state
            MouseButtonState m_leftMouseBtn{ GLFW_MOUSE_BUTTON_LEFT };

            GLFWwindow* m_window = nullptr;
    };
}