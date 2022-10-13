#include "MainWindow.h"
#include "VolumeMetadataDialog.h"

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

	wxBoxSizer* sagittalPanelSizer    = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* sagittalControlsSizer = new wxBoxSizer(wxHORIZONTAL);
	m_sagittalViewSlider = new wxSlider(this, wxID_ANY, 0, 0, 100);
	m_sagittalView       = new VolumeViewCanvas2D(this, m_volumeModel, AnatomicalAxis::SAGITTAL);

	sagittalControlsSizer->Add(new wxStaticText(this, wxID_ANY, "Sagittal"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
	sagittalControlsSizer->Add(m_sagittalViewSlider, 1, wxEXPAND, 5);
	m_sagittalViewSlider->Disable();

	sagittalPanelSizer->Add(sagittalControlsSizer, 0, wxEXPAND);
	sagittalPanelSizer->Add(m_sagittalView, 1, wxEXPAND);

	wxBoxSizer* coronalPanelSizer    = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* coronalControlsSizer = new wxBoxSizer(wxHORIZONTAL);
	m_coronalViewSlider = new wxSlider(this, wxID_ANY, 0, 0, 100);
	m_coronalView       = new VolumeViewCanvas2D(this, m_volumeModel, AnatomicalAxis::CORONAL);

	coronalControlsSizer->Add(new wxStaticText(this, wxID_ANY, "Coronal"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
	coronalControlsSizer->Add(m_coronalViewSlider, 1, wxEXPAND, 5);
	m_coronalViewSlider->Disable();

	coronalPanelSizer->Add(coronalControlsSizer, 0, wxEXPAND);
	coronalPanelSizer->Add(m_coronalView, 1, wxEXPAND);

	wxBoxSizer* horizontalPanelSizer    = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* horizontalControlsSizer = new wxBoxSizer(wxHORIZONTAL);
	m_horizontalViewSlider = new wxSlider(this, wxID_ANY, 0, 0, 100);
	m_horizontalView       = new VolumeViewCanvas2D(this, m_volumeModel, AnatomicalAxis::HORIZONTAL);

	horizontalControlsSizer->Add(new wxStaticText(this, wxID_ANY, "Horizontal"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
	horizontalControlsSizer->Add(m_horizontalViewSlider, 1, wxEXPAND, 5);
	m_horizontalViewSlider->Disable();

	horizontalPanelSizer->Add(horizontalControlsSizer, 0, wxEXPAND);
	horizontalPanelSizer->Add(m_horizontalView, 1, wxEXPAND);

	m_rootCanvas3d   = new VolumeViewCanvas3D(this, m_volumeModel);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* viewContainerSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* bottomViewsSizer   = new wxBoxSizer(wxHORIZONTAL);

	bottomViewsSizer->Add(sagittalPanelSizer, 1, wxEXPAND);
	bottomViewsSizer->Add(coronalPanelSizer, 1, wxEXPAND);
	bottomViewsSizer->Add(horizontalPanelSizer, 1, wxEXPAND);
	
	viewContainerSizer->Add(m_rootCanvas3d, 1, wxEXPAND);
	viewContainerSizer->Add(bottomViewsSizer, 1, wxEXPAND);

	//rootSizer->Add(new wxPanel(this, wxID_ANY), 1, wxEXPAND);
	rootSizer->Add(viewContainerSizer, 4, wxEXPAND);

	SetSizerAndFit(rootSizer);
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
	m_sagittalViewSlider->SetMax(m_sagittalView->GetSliceExtent());
	m_sagittalViewSlider->Enable();
	m_sagittalViewSlider->Bind(wxEVT_SLIDER, [&](const wxCommandEvent& e) {
		m_sagittalView->UpdateSliceOffset(m_sagittalViewSlider->GetValue());
	});

	m_coronalViewSlider->SetValue(0);
	m_coronalViewSlider->SetMax(m_coronalView->GetSliceExtent());
	m_coronalViewSlider->Enable();
	m_horizontalViewSlider->Bind(wxEVT_SLIDER, [&](const wxCommandEvent& e) {
		m_horizontalView->UpdateSliceOffset(m_horizontalViewSlider->GetValue());
	});

	m_horizontalViewSlider->SetValue(0);
	m_horizontalViewSlider->SetMax(m_horizontalView->GetSliceExtent());
	m_horizontalViewSlider->Enable();
	m_coronalViewSlider->Bind(wxEVT_SLIDER, [&](const wxCommandEvent& e) {
		m_coronalView->UpdateSliceOffset(m_coronalViewSlider->GetValue());
	});
}

void MainWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void MainWindow::OnLoadDataset(const char* path, const U32 width, const U32 height, const U32 depth)
{
	try {
		VolumeDataset* dataset = new VolumeDataset(path, VolumeDataType::UINT8, {width, height, depth});
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
			OnLoadDataset(fileName.c_str(), metadataDlg->GetWidthInput(), metadataDlg->GetHeightInput(), metadataDlg->GetDepthInput());
		}
	}
	fileDialog->Destroy();
	metadataDlg->Destroy();
}