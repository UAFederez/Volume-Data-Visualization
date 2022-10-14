#include "MainWindow.h"
#include "VolumeMetadataDialog.h"

#include <wx/splitter.h>

MainWindow::MainWindow(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 150))
{
	m_menuBar  = new wxMenuBar();
	m_fileMenu = new wxMenu();

	m_menuBar->Append(m_fileMenu, wxT("File"));
	m_fileMenu->Append(wxID_OPEN, wxT("Open"));
	m_fileMenu->Append(wxID_EXIT, wxT("Quit"));
	
	this->SetMenuBar(m_menuBar);

	Connect(wxID_EXIT, 
			wxEVT_COMMAND_MENU_SELECTED,
		    wxCommandEventHandler(MainWindow::OnQuit));
	Connect(wxID_OPEN, 
		    wxEVT_COMMAND_MENU_SELECTED,
		    wxCommandEventHandler(MainWindow::OnFileOpen));

	InitializeVolumeModel();


	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	wxSplitterWindow* viewsMainSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);

	viewsMainSplitter->SetSashGravity(0.5);
	viewsMainSplitter->SetMinimumPaneSize(20);
	rootSizer->Add(viewsMainSplitter, 1, wxEXPAND, 0);

	// Top panel
	wxPanel* topViewPanel = new wxPanel(viewsMainSplitter, wxID_ANY);
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		topViewPanel->SetSizer(sizer);
		m_rootCanvas3d = new VolumeViewCanvas3D(topViewPanel, m_volumeModel);
		sizer->Add(m_rootCanvas3d, 1, wxEXPAND, 0);
	}
	
	// Bottom views panel
	wxPanel* bottomViewsPanel = new wxPanel(viewsMainSplitter, wxID_ANY);
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		bottomViewsPanel->SetSizer(sizer);

		wxBoxSizer* sagittalPanelSizer    = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* sagittalControlsSizer = new wxBoxSizer(wxHORIZONTAL);
		m_sagittalViewSlider = new wxSlider(bottomViewsPanel, wxID_ANY, 0, 0, 100);
		m_sagittalView       = new VolumeViewCanvas2D(bottomViewsPanel, m_volumeModel, AnatomicalAxis::SAGITTAL);

		sagittalControlsSizer->Add(new wxStaticText(bottomViewsPanel, wxID_ANY, "Sagittal"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
		sagittalControlsSizer->Add(m_sagittalViewSlider, 1, wxEXPAND | wxBOTTOM, 0);
		m_sagittalViewSlider->Disable();

		sagittalPanelSizer->Add(sagittalControlsSizer, 0, wxEXPAND);
		sagittalPanelSizer->Add(m_sagittalView, 1, wxEXPAND);

		wxBoxSizer* coronalPanelSizer    = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* coronalControlsSizer = new wxBoxSizer(wxHORIZONTAL);
		m_coronalViewSlider = new wxSlider(bottomViewsPanel, wxID_ANY, 0, 0, 100);
		m_coronalView       = new VolumeViewCanvas2D(bottomViewsPanel, m_volumeModel, AnatomicalAxis::CORONAL);

		coronalControlsSizer->Add(new wxStaticText(bottomViewsPanel, wxID_ANY, "Coronal"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
		coronalControlsSizer->Add(m_coronalViewSlider, 1, wxEXPAND | wxBOTTOM, 0);
		m_coronalViewSlider->Disable();

		coronalPanelSizer->Add(coronalControlsSizer, 0, wxEXPAND);
		coronalPanelSizer->Add(m_coronalView, 1, wxEXPAND);

		wxBoxSizer* horizontalPanelSizer    = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* horizontalControlsSizer = new wxBoxSizer(wxHORIZONTAL);
		m_horizontalViewSlider = new wxSlider(bottomViewsPanel, wxID_ANY, 0, 0, 100);
		m_horizontalView       = new VolumeViewCanvas2D(bottomViewsPanel, m_volumeModel, AnatomicalAxis::HORIZONTAL);

		horizontalControlsSizer->Add(new wxStaticText(bottomViewsPanel, wxID_ANY, "Horizontal"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
		horizontalControlsSizer->Add(m_horizontalViewSlider, 1, wxEXPAND | wxBOTTOM, 0);
		m_horizontalViewSlider->Disable();

		horizontalPanelSizer->Add(horizontalControlsSizer, 0, wxEXPAND, 0);
		horizontalPanelSizer->Add(m_horizontalView, 1, wxEXPAND, 0);

		sizer->Add(sagittalPanelSizer, 1, wxEXPAND, 0);
		sizer->Add(coronalPanelSizer, 1, wxEXPAND, 0);
		sizer->Add(horizontalPanelSizer, 1, wxEXPAND, 0);
	}

	viewsMainSplitter->SplitHorizontally(topViewPanel, bottomViewsPanel);

	SetSizerAndFit(rootSizer);
	rootSizer->SetSizeHints(this);
}

void MainWindow::InitializeVolumeModel()
{
	wxGLCanvas * canvas = new wxGLCanvas(this, wxID_ANY);
	
	m_volumeModel = std::make_shared<VolumeModel>();
	
	m_volumeModel->m_sharedContext = std::make_shared<wxGLContext>(canvas);
	m_volumeModel->m_dataset		  = nullptr;
	m_volumeModel->m_texture		  = {};

	canvas->SetCurrent(*m_volumeModel->m_sharedContext);

	glewExperimental = GL_TRUE;
	GLenum status    = glewInit();
	if (status != GLEW_OK)
	{
		const GLubyte* msg = glewGetErrorString(status);
		throw std::exception(reinterpret_cast<const char*>(msg));
	}

	canvas->Destroy();
}

void MainWindow::UpdateVolumeModel(VolumeDataset* dataset)
{
	m_volumeModel->UpdateDataset(dataset, this);
	bool isValid = true;

	if (isValid)
	{
		m_rootCanvas3d->Init();
		m_sagittalView->Init();
		m_coronalView->Init();
		m_horizontalView->Init();
	}

	m_sagittalViewSlider->SetValue(0);
	m_sagittalViewSlider->SetMax(m_sagittalView->GetSliceExtent() - 1);
	m_sagittalViewSlider->Enable();
	m_sagittalViewSlider->Bind(wxEVT_SLIDER, [&](const wxCommandEvent& e) {
		m_sagittalView->UpdateSliceOffset(m_sagittalViewSlider->GetValue());
	});

	m_coronalViewSlider->SetValue(0);
	m_coronalViewSlider->SetMax(m_coronalView->GetSliceExtent() - 1);
	m_coronalViewSlider->Enable();
	m_horizontalViewSlider->Bind(wxEVT_SLIDER, [&](const wxCommandEvent& e) {
		m_horizontalView->UpdateSliceOffset(m_horizontalViewSlider->GetValue());
	});

	m_horizontalViewSlider->SetValue(0);
	m_horizontalViewSlider->SetMax(m_horizontalView->GetSliceExtent() - 1);
	m_horizontalViewSlider->Enable();
	m_coronalViewSlider->Bind(wxEVT_SLIDER, [&](const wxCommandEvent& e) {
		m_coronalView->UpdateSliceOffset(m_coronalViewSlider->GetValue());
	});
}

void MainWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void MainWindow::OnLoadDataset(const char* path, const VolumeDataType type, const U32 width, const U32 height, const U32 depth)
{
	try {
		VolumeDataset* dataset = new VolumeDataset(path, type, {width, height, depth});
		UpdateVolumeModel(dataset);
	}
	catch (std::exception& e) {
		wxLogMessage("%s", e.what());
	}
}

void MainWindow::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog* fileDialog = new wxFileDialog(this);
	VolumeMetadataDialog* metadataDlg = new VolumeMetadataDialog(wxString("Please specify the metadata"));
	if (fileDialog->ShowModal() == wxID_OK)
	{
		wxString fileName = fileDialog->GetPath();
		if (metadataDlg->ShowModal() == wxID_OK)
		{
			OnLoadDataset(fileName.c_str(), 
						  metadataDlg->GetDataType(),
						  metadataDlg->GetWidthInput(), 
						  metadataDlg->GetHeightInput(), 
						  metadataDlg->GetDepthInput());
		}
	}
	fileDialog->Destroy();
	metadataDlg->Destroy();
}