#include "VolumeViewWindow3D.h"

#include <vector>
#include <cstdint>

#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace vr
{
    void VolumeViewWindow3D::Initialize()
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
        m_volumeBoundsShader    = Shader("shaders/volume_raycast_vert.glsl", "shaders/volume_raycast_border_frag.glsl");
    }

    void VolumeViewWindow3D::RenderUI(GLFWwindow* window)
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

            if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsWindowHovered())
            {
                m_distAlongZoomAxis = std::max(0.0f, std::min(m_distAlongZoomAxis - ImGui::GetIO().MouseWheel, 100.0f));
            }

            if (m_distAlongZoomAxis != m_prevDistAlongZoomAxis)
            {
                Refresh();
                m_prevDistAlongZoomAxis = m_distAlongZoomAxis;
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
                        Refresh();
                        

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

            if (m_prevHeight != currHeight || m_prevWidth != currWidth)
                Refresh();

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
                    
                    // The minimum distance that the viewer can be without falling within the box when it is rotated around
                    // In this case, this is length of the diagonal of the volume itself, accounting for spacing
                    // the division by 2.0 is because the volume is centered at the origin
                    const auto minDistance  = glm::length(sizeV3 * spacing / 2.0f);
                    const auto MAX_DISTANCE = 1500.0f;

                    m_cameraPos     = glm::vec3(0.0f, 0.0f, minDistance + (m_distAlongZoomAxis * MAX_DISTANCE / 100.0f));
                    m_projectionMat = glm::perspective(glm::radians(45.0f), aspect, 0.1f, (minDistance + MAX_DISTANCE) * 2.0f);
                    m_viewMat       = glm::lookAt(m_cameraPos, glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f));

                    m_modelScaleMat = glm::scale(glm::mat4(1.0f), sizeV3 * spacing);
                    m_rotationMat   = m_currRotation;

                    glBindVertexArray(m_volumeVao);

                    m_volumeBoundsShader.UseProgram();
                    m_volumeBoundsShader.SetMatrix4x4("model", m_modelScaleMat);
                    m_volumeBoundsShader.SetMatrix4x4("view", m_viewMat);
                    m_volumeBoundsShader.SetMatrix4x4("rotation", m_rotationMat);
                    m_volumeBoundsShader.SetMatrix4x4("projection", m_projectionMat);
                    m_volumeBoundsShader.SetVector3("origin", origin);

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

                    Shader* m_currShader = m_projMethod == ProjectionMethod::COMPOSITE ? 
                                                &m_volumeShaderComposite : 
                                                &m_volumeShaderSimple;

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_3D, m_volumeModel->GetTextureID());

                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, m_volumeModel->GetTransferTextureID("GreenRed"));


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

                    


                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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

    VolumeViewWindow3D::~VolumeViewWindow3D()
    {
        glDeleteVertexArrays(1, &m_volumeVao);
        glDeleteBuffers(1, &m_volumeVbo);
        glDeleteBuffers(1, &m_volumeEbo);
        glDeleteTextures(1, &m_screenColor);
        glDeleteFramebuffers(1, &m_framebuffer);
        glDeleteRenderbuffers(1, &m_screenRbo);
    }


    void VolumeViewWindow3D::UpdateProjectionMethod(ProjectionMethod newMethod, const float firstHitThreshold)
    {
        m_projMethod = newMethod;
        m_firstHitThreshold = firstHitThreshold;
        Refresh();
    }
}
