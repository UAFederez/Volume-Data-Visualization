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
	void OnLoadDataset(const char* path);
private:
	// Menu bar 
	wxMenuBar* m_menuBar  = nullptr;
	wxMenu*	   m_fileMenu = nullptr;

	// Renderer

	VolumeViewCanvas3D* m_rootCanvas3d   = nullptr;
	VolumeViewCanvas2D* m_sagittalView   = nullptr;
	VolumeViewCanvas2D* m_horizontalView = nullptr;
	VolumeViewCanvas2D* m_coronalView    = nullptr;

	std::shared_ptr<wxGLContext> m_sharedContext = nullptr;
	
	// Dataset
	std::unique_ptr<VolumeDataset> m_dataset = nullptr;
	
};

// Reserved IDs for GUI controls
enum {
	MAIN_3D_VIEWPORT_ID = 1612,
	CROSS_SECTION_1_ID  = MAIN_3D_VIEWPORT_ID + 1,
};
