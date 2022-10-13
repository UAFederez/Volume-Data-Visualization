#include "VolumeViewCanvas.h"

VolumeViewCanvas::VolumeViewCanvas(
		wxWindow* parent   , const std::shared_ptr<VolumeModel> model     ,
		wxWindowID id      , const int* attribList  , const wxPoint& pos                   ,
		const wxSize& size , const wxString& name   , long style                           , 
        const wxPalette palette) :
	wxGLCanvas   (parent, id, attribList, pos, size, style, name, palette),
	m_volumeModel(model)
{
	Init();
}


VolumeViewCanvas::~VolumeViewCanvas()
{

}