#include "VolumeViewWindow2D.h"
#include <imgui/imgui.h>

namespace vr
{
    void VolumeViewWindow2D::Initialize()
    {
        // TODO: refactor into Initialize() that gets called from the base class
        // since this is just the steps to setup the framebuffer.
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
        

        const float imageQuadVertices[] = {
             0.0f,  0.0f,  0.0f, 0.0f, // Lower left
             1.0f,  0.0f,  1.0f, 0.0f, // Lower right
             1.0f,  1.0f,  1.0f, 1.0f, // Upper right
             0.0f,  1.0f,  0.0f, 1.0f, // Upper left
        };

        const uint32_t imageQuadIndices[] = { 
            0, 1, 2, 0, 2, 3 
        };

        glGenVertexArrays(1, &m_volumeImageVao);
        glGenBuffers(1, &m_volumeImageVbo);
        glGenBuffers(1, &m_volumeImageEbo);

        glBindVertexArray(m_volumeImageVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_volumeImageVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(imageQuadVertices), imageQuadVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_volumeImageEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(imageQuadIndices), imageQuadIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        m_imageQuadShader = Shader("shaders/volume_cross_section_vert.glsl", "shaders/volume_cross_section_frag.glsl");
    }

    void VolumeViewWindow2D::RenderUI(GLFWwindow* window)
    {
        ImGui::Begin(m_name.c_str());
        {
            if (m_volumeModel)
            {
                ImGui::SliderInt("Slice index", &m_currSliceIndex, 0, m_sliceExtent);

                if (m_prevSliceIndex != m_currSliceIndex)
                {
                    Refresh();
                    m_prevSliceIndex = m_currSliceIndex;
                }
            }

            ImVec2 avail = ImGui::GetContentRegionAvail();

            m_leftButtonState.HandleInput(window);

            uint32_t currWidth  = (uint32_t) avail.x;
            uint32_t currHeight = (uint32_t) avail.y;

            if(m_prevHeight != currHeight || m_prevWidth != currWidth)
                Refresh();

            if (!m_isCacheValid)
            {
                UpdateScreenFrame(currWidth, currHeight);

                glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
                glViewport(0, 0, currWidth, currHeight);
                glDisable(GL_DEPTH_TEST);
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                const float aspect = (float) currWidth / (float) currHeight;

                if (m_volumeModel != nullptr)
                {
                    glm::vec2 dimension = glm::vec2(0.0f);
                    
                    glm::vec3 x_permute = glm::vec3(0.0f);
                    glm::vec3 y_permute = glm::vec3(0.0f);
                    glm::vec3 z_permute = glm::vec3(0.0f);
                    
                    switch (m_axis)
                    {
                        case AnatomicalAxis::CORONAL:
                        {
                            dimension = glm::vec2(m_volumeModel->GetDataSize()[0], m_volumeModel->GetDataSize()[1]);
                            m_sliceExtent = (I32) m_volumeModel->GetDataSize()[2] - 1;

                            // Permute [ x, y, z ] -> [ x, y, z ]
                            x_permute = glm::vec3(1.0f, 0.0f, 0.0f);
                            y_permute = glm::vec3(0.0f, 1.0f, 0.0f);
                            z_permute = glm::vec3(0.0f, 0.0f, 1.0f);
                        } break;
                        case AnatomicalAxis::SAGITTAL:
                        {
                            dimension = glm::vec2(m_volumeModel->GetDataSize()[2], m_volumeModel->GetDataSize()[1]);
                            m_sliceExtent = (I32) m_volumeModel->GetDataSize()[0] - 1;

                            // Permute [ x, y, z ] -> [ z, y, x ]
                            x_permute = glm::vec3(0.0f, 0.0f, 1.0f);
                            y_permute = glm::vec3(0.0f, 1.0f, 0.0f);
                            z_permute = glm::vec3(1.0f, 0.0f, 0.0f);
                        } break;
                        case AnatomicalAxis::HORIZONTAL:
                        {
                            dimension = glm::vec2(m_volumeModel->GetDataSize()[0], m_volumeModel->GetDataSize()[2]);
                            m_sliceExtent = (I32) m_volumeModel->GetDataSize()[1] - 1;

                            // Permute [ x, y, z ] -> [ z, y, x ]
                            x_permute = glm::vec3(1.0f, 0.0f, 0.0f);
                            y_permute = glm::vec3(0.0f, 0.0f, 1.0f);
                            z_permute = glm::vec3(0.0f, 1.0f, 0.0f);
                        } break;
                    }

                    glm::mat3 uvPermutationMatrix = glm::mat3(x_permute, y_permute, z_permute);

                    
                    const float scaleFactor = dimension.x >= dimension.y ? (currWidth / dimension.x) : (currHeight / dimension.y);
                    const glm::vec2 imageDimensions = glm::vec2(dimension.x * scaleFactor, dimension.y * scaleFactor);;
                    
                    // Once the image has been rescaled properly, it must then be centered to the screen
                    const glm::vec3 vecToTranslateCenter = glm::vec3((currWidth - imageDimensions.x) / 2.0, (currHeight - imageDimensions.y) / 2.0, 0.0);
                    const glm::mat4 imageCenterTranslate = glm::translate(glm::mat4(1.0f), vecToTranslateCenter);

                    m_modelScaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(imageDimensions, 1.0));
                    m_viewMat       = imageCenterTranslate;
                    m_projectionmat = glm::ortho(0.0f, (float) currWidth, (float) currHeight, 0.0f, -1.0f, 1.0f);

                    // Since two textures -- the 3d volume and 2d transfer function texture -- are
                    // being used, OpenGL requires that both must be assigned to unique texture identifiers or 'units'
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_3D, m_volumeModel->GetTextureID());

                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, m_volumeModel->GetTransferTextureID("GreenRed"));

                    m_imageQuadShader.UseProgram();

                    // Set the transformation matrices so that the geometry is displayed correctly
                    // i.e., that how the model is translated and scaled corresponds to the current
                    // user zoom and camera pan for this panel
                    m_imageQuadShader.SetMatrix4x4("model", m_modelScaleMat);
                    m_imageQuadShader.SetMatrix4x4("view", m_viewMat);
                    m_imageQuadShader.SetMatrix4x4("projection", m_projectionmat);
            
                    // Set necessary values to be used by the fragment shader
                    // minValue and maxValue are used to determine the interpolation between the colors in the transfer function
                    // texture3d is the actual 3d texture for the dataset
                    // transferTexture is the texture representing the transfer function 
                    m_imageQuadShader.SetFloat("minValue", (float) m_volumeModel->GetMinInDoubleRange());
                    m_imageQuadShader.SetFloat("maxValue", (float) m_volumeModel->GetMaxInDoubleRange());
                    m_imageQuadShader.SetInt("texture3d", 0);
                    m_imageQuadShader.SetInt("transferTexture", 1);

                    // TODO: 
                    // Reconsider in the future especially if there are plans to allow for viewing cross-sections 
                    // from arbitrary axes
                   
                    // Since the volume cross-sections are gathered from the principal axes, then
                    // a possible solution to change UV coordinates depending on the axis is to multiply
                    // the UV coordinates by a permutation matrix. 
                    glm::mat3 permutation = glm::mat3(1.0f);

                    m_imageQuadShader.SetMatrix3x3("uvPermutation", uvPermutationMatrix);
                    m_imageQuadShader.SetFloat("sliceOffset", (float) m_currSliceIndex / (float) m_sliceExtent);

                    glBindVertexArray(m_volumeImageVao);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
}

