#pragma once

#include <memory>
#include <GL/glew.h>
#include <wx/glcanvas.h>
#include "VolumeDataset.h"

class VolumeViewport3D : public wxGLCanvas
{
public:
	VolumeViewport3D(wxWindow*  parent,
		const wxWindowID id		 = wxID_ANY,
		const int* attribList	 = NULL,
		const wxPoint& pos		 = wxDefaultPosition,
		const wxSize&  size		 = wxDefaultSize, 
		long  style			     = 0L,
		const wxString& name	 = wxT("Renderer"),
		const wxPalette& palette = wxNullPalette);
	~VolumeViewport3D();

	VolumeViewport3D(const VolumeViewport3D&) = delete;
	VolumeViewport3D(VolumeViewport3D&&)	  = delete;
	VolumeViewport3D& operator=(const VolumeViewport3D&) = delete;
	VolumeViewport3D& operator=(VolumeViewport3D&&)		 = delete;

	void UpdateDataset(const VolumeDataset& dataset);
private:
	void InitializeOpenGL();
	void InitializeRenderer();

	// Event handlers
	void OnPaint	(wxPaintEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseUp  (wxMouseEvent& event);


	std::unique_ptr<wxGLContext> m_glContext;
};