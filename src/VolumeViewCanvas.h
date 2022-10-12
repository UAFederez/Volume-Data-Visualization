#pragma once

#include <GL/glew.h>
#include <wx/glcanvas.h>
#include <memory>

#include "VolumeDataset.h"

class VolumeViewCanvas : public wxGLCanvas
{
    public:
        VolumeViewCanvas(wxWindow* parent                      , VolumeDataset* dataset                    ,
                         std::shared_ptr<wxGLContext> context  , wxWindowID id = wxID_ANY                  ,
                         const int* attribList = nullptr       , const wxPoint& pos    = wxDefaultPosition ,
                         const wxSize&  size   = wxDefaultSize , const wxString& name  = L"OpenGLCanvas"   ,
                         long style	= 0L                       , const wxPalette palette = wxNullPalette);

        virtual ~VolumeViewCanvas();

        std::shared_ptr<wxGLContext> CreateContextFromThisCanvas(bool willReinitialize = true)
        {
            m_glContext = std::make_shared<wxGLContext>(this);
            SetCurrent(*m_glContext);

            glewExperimental = GL_TRUE;
            GLenum status    = glewInit();
            if (status != GLEW_OK)
            {
                const GLubyte* msg = glewGetErrorString(status);
                throw std::exception(reinterpret_cast<const char*>(msg));
            }

            if (willReinitialize)
            {
                Init();
            }

            return m_glContext;
        }

        void SetGLContext(std::shared_ptr<wxGLContext> newContext)
        {
            m_glContext = newContext;
        }

        void UpdateDataset(const VolumeDataset* newDataset)
        {
            m_dataset = newDataset;
            Deallocate();
            Init();
        }

        VolumeViewCanvas(const VolumeViewCanvas&)            = delete;
        VolumeViewCanvas& operator=(const VolumeViewCanvas&) = delete;

        GLuint GetTextureId() const { return m_textureId; }
        void   SetTextureId(GLuint texId)
        {
            m_textureId = texId;
            Refresh();
        }

    protected:
        GLuint m_textureId;

        std::shared_ptr<wxGLContext> m_glContext = nullptr; 
        const VolumeDataset* m_dataset = nullptr;
        
        virtual void Deallocate() {};
        virtual void Init() {};
};