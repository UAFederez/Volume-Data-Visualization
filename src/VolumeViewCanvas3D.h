#pragma once

#include "VolumeViewCanvas.h"
#include "Shader.h"
#include <glm/glm.hpp>

struct VolumeBounds
{
    U32 edgeIndices[36];
    glm::vec3 vertices[8];
    glm::vec3 transformedVerts[8];
    glm::mat4 rotation;
};

class VolumeViewCanvas3D final : public VolumeViewCanvas
{
public:
    VolumeViewCanvas3D(wxWindow* parent, const std::shared_ptr<VolumeModel> model);

    virtual ~VolumeViewCanvas3D() { Deallocate(); }

    void Init() override;

protected:
    void Deallocate() override;
private:
    void Render(wxPaintEvent& evt);
    void HandleLeftClick(wxMouseEvent& evt);
    void HandleLeftRelease(wxMouseEvent& evt);
    void HandleMouseMove(wxMouseEvent& evt);
    void HandleMouseScroll(wxMouseEvent& evt);

    VolumeBounds m_volumeBounds = {};
    GLuint m_volBoundsVao, m_volBoundsVbo, m_volBoundsEbo;

    Shader m_volumeShader;
    Shader m_volumeBoundsShader;

    /**
    * User input data
    **/
    bool isMouseDown = false;

    // Camera
    glm::vec3 m_cameraPos = glm::vec3(0.0f);

    // Trackball-style rotation
    glm::vec3 m_mouseHemi0 = glm::vec3(0.0f);
    glm::vec3 m_mouseHemi1 = glm::vec3(0.0f);
    glm::vec3 m_rotateAxis = glm::vec3(1.0f);
    glm::mat4 m_previousRotation = glm::mat4(1.0f);
    glm::mat4 m_currentRotation  = glm::mat4(1.0f);

    float m_rotateAngle = 0.0f;
};

VolumeBounds ConstructVolumeBoundsFromDataset(const VolumeDataset* data);