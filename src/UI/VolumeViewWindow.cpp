#include "VolumeViewWindow.h"
#include <vector>
#include <cstdint>

#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace vr
{
    void VolumeViewWindow::Initialize()
    {
        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

        glGenTextures(1, &m_screenColor);
        glBindTexture(GL_TEXTURE_2D, m_screenColor);
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, m_prevWidth, m_prevHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_screenColor, 0);
        
        glGenRenderbuffers(1, &m_screenRbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_screenRbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_prevWidth, m_prevHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_screenRbo);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Error: Incomplete framebuffer\n";
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        const float cubeVertices[] = {
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
        };
        
        const uint32_t cubeIndices[36] = {
            0, 1, 3, 0, 3, 2, // Back face
            4, 0, 2, 4, 2, 6, // West face,
            5, 1, 3, 5, 3, 7, // East face
            4, 5, 1, 4, 1, 0, // Bottom
            6, 7, 3, 6, 3, 2, // Top face
            4, 5, 7, 4, 7, 6, // Front face
        };

        glGenVertexArrays(1, &m_volumeVao);
        glGenBuffers(1, &m_volumeVbo);
        glGenBuffers(1, &m_volumeEbo);

        glBindVertexArray(m_volumeVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_volumeVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_volumeEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        m_volumeShaderComposite = Shader("shaders/volume_raycast_vert.glsl", "shaders/volume_raycast_composite_frag.glsl");
        m_volumeShaderSimple    = Shader("shaders/volume_raycast_vert.glsl", "shaders/volume_raycast_frag.glsl");
    }

    void VolumeViewWindow::Refresh()
    {
        m_isCacheValid = false;
    }

    void VolumeViewWindow::UpdateProjectionMethod(ProjectionMethod newMethod, const float firstHitThreshold)
    {
        m_projMethod = newMethod;
        m_firstHitThreshold = firstHitThreshold;
        Refresh();
    }

    void VolumeViewWindow::RenderUI(GLFWwindow* window)
    {
        ImGui::Begin("3D Viewport");
        {
            
            ImVec2 avail = ImGui::GetContentRegionAvail();

            uint32_t currWidth  = (uint32_t) avail.x;
            uint32_t currHeight = (uint32_t) avail.y;

            // Input
            m_leftButtonState.HandleInput(window);


            if (m_leftButtonState.WasReleased())
            {
                m_prevRotation = m_currRotation;
            }


            if (m_leftButtonState.IsDragging() && ImGui::IsWindowFocused())
            {
                auto cursorScreenPos = ImGui::GetCursorScreenPos();
                
                {
                    m_mouseHemi1.x =  (2.0 * (m_leftButtonState.GetButtonX() - cursorScreenPos.x) - avail.x) / (R32) avail.x;
                    m_mouseHemi1.y = -(2.0 * (m_leftButtonState.GetButtonY() - cursorScreenPos.y) - avail.y) / (R32) avail.y;

                    const R32 lengthSquared = (m_mouseHemi1.x * m_mouseHemi1.x) + (m_mouseHemi1.y * m_mouseHemi1.y);

                    if (lengthSquared <= 1.0f)
                        m_mouseHemi1.z = std::sqrt(1 - lengthSquared);
                    else
                        m_mouseHemi1 = glm::normalize(m_mouseHemi1);
                }
                
                {
                    glm::vec2 currDragVec = glm::vec2(m_leftButtonState.GetDragVecX(), m_leftButtonState.GetDragVecY());
                    if (currDragVec != m_prevDragVec)
                        m_isCacheValid = false;

                    m_mouseHemi0.x =  (2.0 * (m_leftButtonState.GetButtonX() - m_leftButtonState.GetDragVecX() - cursorScreenPos.x) - avail.x) / (R32) avail.x;
                    m_mouseHemi0.y = -(2.0 * (m_leftButtonState.GetButtonY() - m_leftButtonState.GetDragVecY() - cursorScreenPos.y) - avail.y) / (R32) avail.y;

                    const R32 lengthSquared = (m_mouseHemi0.x * m_mouseHemi0.x) + (m_mouseHemi0.y * m_mouseHemi0.y);

                    if (lengthSquared <= 1.0f)
                        m_mouseHemi0.z = std::sqrt(1 - lengthSquared);
                    else
                        m_mouseHemi0 = glm::normalize(m_mouseHemi0);

                    m_prevDragVec.x = m_leftButtonState.GetDragVecX();
                    m_prevDragVec.y = m_leftButtonState.GetDragVecY();
                }                     

                const float rotateSpeed = 75.0f;

                m_rotateAxis  = glm::normalize(glm::cross(m_mouseHemi0, m_mouseHemi1));
                m_rotateAngle = glm::acos(glm::dot(m_mouseHemi0, m_mouseHemi1)) * rotateSpeed;

                m_currRotation = glm::rotate(m_prevRotation, 
                                             glm::radians(m_rotateAngle), 
                                             glm::vec3(glm::transpose(m_prevRotation) * glm::vec4(m_rotateAxis, 1.0f)));
                
            }


            if(m_prevHeight != currHeight || m_prevWidth != currWidth)
                m_isCacheValid = false;

            if (!m_isCacheValid)
            {
                UpdateScreenFrame(currWidth, currHeight);

                glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
                glViewport(0, 0, currWidth, currHeight);
                glEnable(GL_DEPTH_TEST);
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                const float aspect = (float) currWidth / (float) currHeight;

                if (m_volumeModel != nullptr)
                {
                    // Find optimal camera position
                    const auto dataSize     = m_volumeModel->GetDataSize();
                    const auto dataSpacing  = m_volumeModel->GetDataSpacing();
                    const glm::vec3 sizeV3  = glm::vec3(dataSize[0], dataSize[1], dataSize[2]);

                    // Rescale the model so that spacing x is 1
                    const glm::vec3 spacing = glm::vec3(dataSpacing[0], dataSpacing[1], dataSpacing[2]) / (R32) dataSpacing[0];

                    const glm::vec3 origin  = -0.5f * (sizeV3 * spacing);
                    const auto minDistance  = glm::length(sizeV3 * spacing) + 150.0f;

                    m_cameraPos     = glm::vec3(0.0f, 0.0f, minDistance);
                    m_projectionMat = glm::perspective(glm::radians(45.0f), aspect, 0.1f, (minDistance + 500.0f) * 2.0f);
                    m_viewMat       = glm::lookAt(m_cameraPos, glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f));

                    m_modelScaleMat = glm::scale(glm::mat4(1.0f), sizeV3 * spacing);
                    m_rotationMat   = m_currRotation;

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_3D, m_volumeModel->GetTextureID());

                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, m_volumeModel->GetTransferTextureID("GreenRed"));

                    Shader* m_currShader = m_projMethod == ProjectionMethod::COMPOSITE ? 
                                                &m_volumeShaderComposite : 
                                                &m_volumeShaderSimple;

                    m_currShader->UseProgram();
                    m_currShader->SetMatrix4x4("model", m_modelScaleMat);
                    m_currShader->SetMatrix4x4("view", m_viewMat);
                    m_currShader->SetMatrix4x4("rotation", m_rotationMat);
                    m_currShader->SetMatrix4x4("projection", m_projectionMat);
                    
                    m_currShader->SetFloat("minValue", (float) m_volumeModel->GetMinInDoubleRange());
                    m_currShader->SetFloat("maxValue", (float) m_volumeModel->GetMaxInDoubleRange());
                    
                    U32 projIdx = std::underlying_type<ProjectionMethod>::type(m_projMethod);
                    m_currShader->SetUint("projMethod", projIdx);
                    m_currShader->SetFloat("firstHitThreshold", m_firstHitThreshold);

                    m_currShader->SetInt("texture3d", 0);
                    m_currShader->SetInt("transferTexture", 1);
                    
                    m_currShader->SetVector3("origin", origin);
                    m_currShader->SetVector3("volumeSize", sizeV3 * spacing);
                    m_currShader->SetVector3("cameraPosition", m_cameraPos);

                    glBindVertexArray(m_volumeVao);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);;
                }
                
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            m_prevHeight = (uint32_t) avail.y;
            m_prevWidth  = (uint32_t) avail.x;

            ImGui::Image(
                (ImTextureID) m_screenColor, 
                { avail.x, avail.y },
                ImVec2(0, 1),
                ImVec2(1, 0)
            );

        }
        ImGui::End();
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

    VolumeViewWindow::~VolumeViewWindow()
    {
        glDeleteVertexArrays(1, &m_volumeVao);
        glDeleteBuffers(1, &m_volumeVbo);
        glDeleteBuffers(1, &m_volumeEbo);
        glDeleteTextures(1, &m_screenColor);
        glDeleteFramebuffers(1, &m_framebuffer);
        glDeleteRenderbuffers(1, &m_screenRbo);
    }
}