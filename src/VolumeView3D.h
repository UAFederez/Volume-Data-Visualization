#pragma once

#include <memory>
#include <GL/glew.h>
#include <wx/glcanvas.h>
#include "VolumeDataset.h"
#include "Shader.h"


struct VolumeBounds
{
	U32 edgeIndices[36];
	glm::vec3 vertices[8];
	glm::vec3 transformedVerts[8];
	glm::mat4 rotation;
};

VolumeBounds ConstructVolumeBoundsFromDataset(const VolumeDataset* data);

class VolumeView3D : public wxGLCanvas
{
public:
	VolumeView3D(wxWindow* parent,
				 VolumeDataset* dataset = nullptr,
				 wxWindowID id = wxID_ANY,
				 const int* attribList = nullptr,
				 const wxPoint& pos    = wxDefaultPosition,
				 const wxSize&  size   = wxDefaultSize,
				 const wxString& name  = L"OpenGLCanvas",
				 long style			   = 0L,
				 const wxPalette palette = wxNullPalette);

	VolumeView3D(const VolumeView3D&) = delete;
	VolumeView3D(VolumeView3D&&)	  = delete;
	VolumeView3D& operator=(const VolumeView3D&) = delete;
	VolumeView3D& operator=(VolumeView3D&&)		 = delete;

	void UpdateDataset(VolumeDataset* dataset)
	{
		m_dataset	   = dataset;
		m_volumeBounds = ConstructVolumeBoundsFromDataset(this->m_dataset);
		InitializeGraphics();
		Refresh();
	}

	virtual ~VolumeView3D();
private:
	void InitializeOpenGL();
	void InitializeGraphics();
	void OnLeftClick(wxMouseEvent& event);
	void OnLeftRelease(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnPaint(wxPaintEvent& evt);

	VolumeDataset* m_dataset = nullptr;

	std::unique_ptr<wxGLContext> m_glContext;

	// OpenGL data
	GLuint m_texture3d;
	GLuint m_sliceVao, m_sliceVbo, m_sliceEbo, m_sliceInstanceVbo;
	Shader m_volumeShader;
	Shader m_volumeBoundsShader;

	GLuint m_volBoundsVao, m_volBoundsVbo, m_volBoundsEbo;

	VolumeBounds m_volumeBounds = {};

	glm::mat4 m_viewMatrix = glm::mat4(1.0f);

	/**
	 * User input data
	**/
	bool isMouseDown = false;

	// Trackball-style rotation
	glm::vec3 m_mouseHemi0 = glm::vec3(0.0f);
	glm::vec3 m_mouseHemi1 = glm::vec3(0.0f);
	glm::vec3 m_rotateAxis = glm::vec3(1.0f);
	glm::mat4 m_previousRotation = glm::mat4(1.0f);
	glm::mat4 m_currentRotation  = glm::mat4(1.0f);
	float m_rotateAngle = 0.0f;
};