#pragma once

#include "VolumeViewCanvas.h"
#include "Shader.h"

// TODO: Maybe add future support for arbitrary axes
enum class AnatomicalAxis { CORONAL, SAGITTAL, HORIZONTAL };

class VolumeViewCanvas2D final : public VolumeViewCanvas
{
public:
    VolumeViewCanvas2D(wxWindow* parent, 
        VolumeDataset* dataset, 
        AnatomicalAxis axis,
        std::shared_ptr<wxGLContext> context);

    virtual ~VolumeViewCanvas2D() { Deallocate(); }

protected:
    virtual void Deallocate() override;
    virtual void Init() override;

private:
    void Render(wxPaintEvent& evt);
    void HandleLeftClick(wxMouseEvent& evt);
    void HandleLeftRelease(wxMouseEvent& evt);
    void HandleMouseMove(wxMouseEvent& evt);
    void HandleMouseScroll(wxMouseEvent& evt);

    float m_sliceOffset = 0.0f;

    GLuint m_imageVao, m_imageVbo, m_imageEbo;
    Shader m_imageShader;
    AnatomicalAxis m_axis;
};
