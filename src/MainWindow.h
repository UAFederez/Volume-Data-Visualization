#pragma once

#include <wx/wx.h>
#include <string>
#include "VolumeDataset.h"

#include "VolumeViewCanvas.h"
#include "VolumeViewCanvas3D.h"
#include "VolumeViewCanvas2D.h"

class MainWindow : public wxFrame
{
public:
	MainWindow(const wxString& title);
	
	MainWindow(const MainWindow&) = delete;
	MainWindow(MainWindow&&)	  = delete;
	MainWindow& operator=(const MainWindow&) = delete;
	MainWindow& operator=(MainWindow&&)		 = delete;

	void OnQuit	   (wxCommandEvent& evt);
	void OnFileOpen(wxCommandEvent& evt);
	void OnLoadDataset(const char* path, const VolumeDataType type, 
					   const U32 width , const U32 height, const U32 depth, 
					   const R64 spaceX, const R64 spaceY, const R64 spaceZ);
private:
	// Menu bar 
	wxMenuBar* m_menuBar  = nullptr;
	wxMenu*	   m_fileMenu = nullptr;

	GLuint m_textureId;

	void InitializeVolumeModel();
	void UpdateProjectionMethod(wxCommandEvent& e);
	void UpdateVolumeModel(VolumeDataset* newDataset);

	std::shared_ptr<VolumeModel> m_volumeModel = {};

	// Renderer
	VolumeViewCanvas*   m_rootCanvas     = nullptr;
	VolumeViewCanvas3D* m_rootCanvas3d   = nullptr;
	VolumeViewCanvas2D* m_sagittalView   = nullptr;
	VolumeViewCanvas2D* m_horizontalView = nullptr;
	VolumeViewCanvas2D* m_coronalView    = nullptr;
	
	wxPanel* m_sagittalViewPanel   = nullptr;
	wxPanel* m_coronalViewPanel    = nullptr;
	wxPanel* m_horizontalViewPanel = nullptr;

	wxComboBox* m_renderMethodSelect = nullptr;
	wxCheckBox* m_boxVisibleCheck = nullptr;
	wxSlider*   m_firstHitThresholdSlider = nullptr;

	wxSlider* m_sagittalViewSlider   = nullptr;
	wxSlider* m_coronalViewSlider    = nullptr;
	wxSlider* m_horizontalViewSlider = nullptr;
};

// Reserved IDs for GUI controls
enum {
	MAIN_3D_VIEWPORT_ID = 1612,
	CROSS_SECTION_1_ID  = MAIN_3D_VIEWPORT_ID + 1,
};
