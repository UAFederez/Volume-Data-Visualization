#pragma once

#include <GL/glew.h>
#include <wx/glcanvas.h>
#include <memory>
#include <future>

#include "VolumeDataset.h"

struct VolumeTexture
{
    public:
        VolumeTexture() = default;
        VolumeTexture(VolumeDataset* dataset)
        {
            LoadFromDataset(dataset);
        }

        GLuint GetTextureID() const { return m_textureId; }

        void UpdateTexture(VolumeDataset* dataset)
        {
            if (m_isValidTexture)
            {
                glDeleteTextures(1, &m_textureId);
                m_isValidTexture = false;
            }
                
            LoadFromDataset(dataset);
        }
    private:
        void LoadFromDataset(VolumeDataset* dataset)
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
            glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // Push 3D texture data to the GPU
            const U8* data = dataset->RawData();
            wxLogDebug("glGetError: %d", glGetError());
            glTexImage3D(GL_TEXTURE_3D, 
                         0, 
                         GL_LUMINANCE, 
                         dataset->DataSize()[0], 
                         dataset->DataSize()[1], 
                         dataset->DataSize()[2],
                         0,
                         GL_LUMINANCE,
                         GL_UNSIGNED_BYTE, 
                         data);
            wxLogDebug("glGetError: %d", glGetError());
            glBindTexture(GL_TEXTURE_3D, 0);

            m_isValidTexture = true;
        }

        GLuint m_textureId = 0;
        bool   m_isValidTexture = false;
};

struct VolumeModel
{
    void UpdateDataset(VolumeDataset* dataset, wxWindow* parent)
    {
        m_dataset.reset(dataset);
        UpdateTexture(parent);
    }

    void UpdateTexture(wxWindow* parent)
    {
        wxGLCanvas* canvas = new wxGLCanvas(parent, wxID_ANY);
        canvas->SetCurrent(*m_sharedContext);
        m_texture.UpdateTexture(m_dataset.get());
        canvas->Destroy();
    }

    VolumeTexture  m_texture;
    std::unique_ptr<VolumeDataset> m_dataset       = nullptr;
    std::shared_ptr<wxGLContext>   m_sharedContext = nullptr;
};

class VolumeViewCanvas : public wxGLCanvas
{
    public:
        VolumeViewCanvas(wxWindow* parent                      , const std::shared_ptr<VolumeModel> model                        ,
                         wxWindowID id = wxID_ANY,  const int* attribList = nullptr       , const wxPoint& pos    = wxDefaultPosition ,
                         const wxSize&  size   = wxDefaultSize , const wxString& name  = L"OpenGLCanvas"   ,
                         long style	= 0L                       , const wxPalette palette = wxNullPalette);

        virtual ~VolumeViewCanvas();

        VolumeViewCanvas(const VolumeViewCanvas&)            = delete;
        VolumeViewCanvas& operator=(const VolumeViewCanvas&) = delete;
        virtual void Init() {};

    protected:
        const std::shared_ptr<VolumeModel> m_volumeModel = nullptr;
        virtual void Deallocate() {};

        bool m_hasPreviousData = false;
        
};