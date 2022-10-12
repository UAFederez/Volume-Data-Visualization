#include "VolumeViewCanvas.h"

VolumeViewCanvas::VolumeViewCanvas(
		wxWindow* parent   , VolumeDataset* dataset , std::shared_ptr<wxGLContext> context ,
		wxWindowID id      , const int* attribList  , const wxPoint& pos                   ,
		const wxSize& size , const wxString& name   , long style                           , 
        const wxPalette palette) :

	wxGLCanvas (parent, id, attribList, pos, size, style, name, palette),
	m_dataset  (dataset),
    m_glContext(context)
{
	Init();
}


VolumeViewCanvas::~VolumeViewCanvas()
{

}