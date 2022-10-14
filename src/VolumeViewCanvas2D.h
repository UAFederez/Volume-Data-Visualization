#pragma once

#include "VolumeViewCanvas.h"
#include "Shader.h"

// TODO: Maybe add future support for arbitrary axes
enum class AnatomicalAxis { CORONAL, SAGITTAL, HORIZONTAL };

class VolumeViewCanvas2D final : public VolumeViewCanvas
{
public:
    VolumeViewCanvas2D(wxWindow* parent, 
        const std::shared_ptr<VolumeModel> model, 
        AnatomicalAxis axis);

    virtual ~VolumeViewCanvas2D() { Deallocate(); }
    virtual void Init() override;

    void UpdateSliceOffset(U32 newSliceIndex)
    {
        m_sliceIndex  = std::min(m_sliceExtent, std::max(0u, newSliceIndex));
        Refresh();
    }

    U32 GetSliceExtent() const { return m_sliceExtent; }

protected:
    virtual void Deallocate() override;
    

private:
    void Render(wxPaintEvent& evt);
    void HandleLeftClick(wxMouseEvent& evt);
    void HandleLeftRelease(wxMouseEvent& evt);
    void HandleMouseMove(wxMouseEvent& evt);
    void HandleMouseScroll(wxMouseEvent& evt);

    U32 m_sliceIndex  = 0;
    U32 m_sliceExtent = 0;
    R32 m_zoomLevel   = 1.0f;
    glm::vec2 m_scaleCenter   = glm::vec2(0.0f);
    glm::vec2 m_clickStart    = glm::vec2(0.0f);

    glm::vec2 m_cameraOffset  = glm::vec2(0.0f);
    glm::mat4 m_cameraTranslate = glm::mat4(1.0f);
    glm::mat4 m_cameraPrevTranslate = glm::mat4(1.0f);

    GLuint m_imageVao, m_imageVbo, m_imageEbo;
    Shader m_imageShader;
    AnatomicalAxis m_axis;
};
