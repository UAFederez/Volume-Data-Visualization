#pragma once

#include <GL/glew.h>
#include <wx/glcanvas.h>
#include <memory>
#include <future>
#include <array>

#include "VolumeModel/VolumeDataset.h"
#include "VolumeModel/TransferFunction1D.h"

struct VolumeTexture
{
    public:
        VolumeTexture() = default;
        VolumeTexture(VolumeDataset* dataset)
        {
            LoadFromDataset(dataset);
        }

        GLuint GetTextureID() const { return m_textureId; }
        void UpdateTexture(VolumeDataset* dataset);
    private:
        void LoadFromDataset(VolumeDataset* dataset);

        GLuint m_textureId = 0;
        bool   m_isValidTexture = false;
};

struct VolumeModel
{
    public:
        VolumeModel()
        {
            
        }
        void UpdateDataset(VolumeDataset* dataset, wxWindow* parent);
        void UpdateTexture(wxWindow* parent);
        void CreateTransferFuncTexture(wxWindow* parent);

        TransferFunction1D m_transferFunction = {};
        VolumeTexture  m_texture;
        std::unique_ptr<VolumeDataset> m_dataset       = nullptr;
        std::shared_ptr<wxGLContext>   m_sharedContext = nullptr;
};

class VolumeViewCanvas : public wxGLCanvas
{
    public:
        VolumeViewCanvas(wxWindow* parent                          ,
                         const std::shared_ptr<VolumeModel> model  ,
                         wxWindowID id = wxID_ANY                  ,
                         const int* attribList = nullptr           ,
                         const wxPoint& pos = wxDefaultPosition    ,
                         const wxSize&  size   = wxDefaultSize     ,
                         const wxString& name  = L"OpenGLCanvas"   ,
                         long style	= 0L                           ,
                         const wxPalette palette = wxNullPalette);

        virtual ~VolumeViewCanvas();

        VolumeViewCanvas(const VolumeViewCanvas&)            = delete;
        VolumeViewCanvas& operator=(const VolumeViewCanvas&) = delete;
        virtual void Init() {};

    protected:
        const std::shared_ptr<VolumeModel> m_volumeModel = nullptr;
        virtual void Deallocate() {};

        bool m_hasPreviousData = false;
        
};
