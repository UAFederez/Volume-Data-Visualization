#include "VolumeViewCanvas.h"

VolumeViewCanvas::VolumeViewCanvas(
		wxWindow* parent      , const std::shared_ptr<VolumeModel> model , wxWindowID id              ,
        const int* attribList , const wxPoint& pos                       , const wxSize& size         ,
        const wxString& name  , long style                               , const wxPalette palette) :
	wxGLCanvas   (parent, id, attribList, pos, size, style, name, palette),
	m_volumeModel(model)
{
	Init();
}


VolumeViewCanvas::~VolumeViewCanvas()
{

}

void VolumeModel::CreateTransferFuncTexture(wxWindow* parent)
{
    wxGLCanvas* canvas = new wxGLCanvas(parent, wxID_ANY, nullptr);
    canvas->SetCurrent(*m_sharedContext);

    // Initialize Data
    m_transferFunction.AddColorStop(0.00f, 0.0f, 0.8f, 1.0f);
    m_transferFunction.AddColorStop(1.00f, 1.0f, 0.8f, 0.0f);

    m_transferFunction.AddOpacityStop(0.00f, 0.00f);
    m_transferFunction.AddOpacityStop(0.25f, 0.00f);
    m_transferFunction.AddOpacityStop(1.00f, 1.00f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const auto textureData = m_transferFunction.CreateImageFromData();

    glGenTextures(1, &m_transferFunction.m_cmapTex);
    glBindTexture(GL_TEXTURE_2D, m_transferFunction.m_cmapTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, (U8*) textureData.data());
    glBindTexture(GL_TEXTURE_2D, 0);
    
    canvas->Destroy();
}

void VolumeModel::UpdateDataset(VolumeDataset* dataset, wxWindow* parent)
{
    m_dataset.reset(dataset);
    UpdateTexture(parent);
}

void VolumeModel::UpdateTexture(wxWindow* parent)
{
    wxGLCanvas* canvas = new wxGLCanvas(parent, wxID_ANY, nullptr);
    canvas->SetCurrent(*m_sharedContext);
    m_texture.UpdateTexture(m_dataset.get());
    canvas->Destroy();
}

void VolumeTexture::UpdateTexture(VolumeDataset* dataset)
{
    if (m_isValidTexture)
    {
        glDeleteTextures(1, &m_textureId);
        m_isValidTexture = false;
    }

    LoadFromDataset(dataset);
}

void VolumeTexture::LoadFromDataset(VolumeDataset* dataset)
{
    const float borderColor[] = {.0f, .0f, .0f, .0f};

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_3D, m_textureId);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set texture parameters
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum type = 0;
    switch (dataset->DataType())
    {
        case VolumeDataType::UINT8 : type = GL_UNSIGNED_BYTE ; break;
        case VolumeDataType::UINT16: type = GL_UNSIGNED_SHORT; break;
        case VolumeDataType::FLOAT : type = GL_FLOAT         ; break;
    }

    // Push 3D texture data to the GPU
    const U8* data = dataset->RawData();
    wxLogDebug("glGetError: %d", glGetError());
    glTexImage3D(GL_TEXTURE_3D, 
        0, 
        GL_COMPRESSED_LUMINANCE, 
        dataset->DataSize()[0], 
        dataset->DataSize()[1], 
        dataset->DataSize()[2],
        0,
        GL_LUMINANCE,
        type, 
        data);
    glGenerateMipmap(GL_TEXTURE_3D);
    wxLogDebug("glGetError: %d", glGetError());
    glBindTexture(GL_TEXTURE_3D, 0);

    m_isValidTexture = true;
}
