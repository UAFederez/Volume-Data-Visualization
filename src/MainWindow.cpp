#include "MainWindow.h"
#include "VolumeMetadataDialog.h"

#include <wx/splitter.h>
#include <wx/notebook.h>

void MainWindow::OnPaint(wxPaintEvent& e)
{ 
    try {
        if(m_volumeModel->m_sharedContext == nullptr)
        {
            InitializeVolumeModel();
            m_rootCanvas3d->Init();
            m_sagittalView->Init();
            m_coronalView->Init();
            m_horizontalView->Init();
        }
    } catch(std::exception& e) {
        wxLogDebug("OnPaint %s", e.what());
    }
}

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

	m_volumeModel = std::make_shared<VolumeModel>();
    this->Bind(wxEVT_PAINT, &MainWindow::OnPaint, this);

    try {
        wxSplitterWindow* rootSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
        wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
        
        rootSplitter->SetMinimumPaneSize(20);
        rootSizer->Add(rootSplitter, 1, wxEXPAND, 0);

        wxBoxSizer* viewSizer = new wxBoxSizer(wxVERTICAL);
        wxSplitterWindow* viewsMainSplitter = new wxSplitterWindow(rootSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);

        viewsMainSplitter->SetSashGravity(0.5);
        viewsMainSplitter->SetMinimumPaneSize(20);
        viewSizer->Add(viewsMainSplitter, 1, wxEXPAND, 0);

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

            sizer->Add(sagittalPanelSizer, 1, wxEXPAND, 5);
            sizer->Add(coronalPanelSizer, 1, wxEXPAND, 5);
            sizer->Add(horizontalPanelSizer, 1, wxEXPAND, 5);
        }

        viewsMainSplitter->SplitHorizontally(topViewPanel, bottomViewsPanel);

        wxPanel* sidebar  = new wxPanel(rootSplitter, wxID_ANY);
        wxBoxSizer* sidebarSizer = new wxBoxSizer(wxVERTICAL);
        sidebar->SetSizerAndFit(sidebarSizer);

        wxNotebook* notebook = new wxNotebook(sidebar, wxID_ANY);

        wxPanel* renderSettings = new wxPanel(notebook, wxID_ANY);
        wxFlexGridSizer* renderSettingsSizer = new wxFlexGridSizer(3, 2, 0, 0);

        renderSettings->SetSizerAndFit(renderSettingsSizer);
        {
            wxArrayString renderOptions = wxArrayString();
            renderOptions.Add(wxString("Maximum Intensity Projection"));
            renderOptions.Add(wxString("Average intensity"));
            renderOptions.Add(wxString("First hit"));
            renderOptions.Add(wxString("Composite"));

            m_renderMethodSelect = new wxComboBox(renderSettings, wxID_ANY, "Maximum Intensity Projection", wxDefaultPosition, wxDefaultSize, renderOptions, wxCB_READONLY);
            m_renderMethodSelect->Disable();
            m_firstHitThresholdSlider = new wxSlider(renderSettings, wxID_ANY, 0, 0, 100);
            m_firstHitThresholdSlider->Disable();
            m_boxVisibleCheck = new wxCheckBox(renderSettings, wxID_ANY, wxT("Is visible"));
            m_boxVisibleCheck->Disable();
            
            renderSettingsSizer->Add(new wxStaticText(renderSettings, wxID_ANY, "Projection method"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
            renderSettingsSizer->Add(m_renderMethodSelect, 0, wxEXPAND, 0);
            renderSettingsSizer->Add(new wxStaticText(renderSettings, wxID_ANY, "First hit threshold"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
            renderSettingsSizer->Add(m_firstHitThresholdSlider, 0, wxEXPAND, 0);
            renderSettingsSizer->Add(new wxStaticText(renderSettings, wxID_ANY, "Is box visible?"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
            renderSettingsSizer->Add(m_boxVisibleCheck, 0, wxEXPAND, 0);
        }

        notebook->AddPage(renderSettings, "Rendering");
        notebook->AddPage(new wxPanel(notebook, wxID_ANY), "Dataset Properties");
        

        sidebarSizer->Add(notebook, 1, wxEXPAND, 0);

        rootSplitter->SplitVertically(sidebar, viewsMainSplitter);
        rootSplitter->SetSashGravity(0.05f);

        SetSizerAndFit(rootSizer);
        rootSizer->SetSizeHints(this);
    } catch(std::exception& e) {
        wxLogDebug(e.what());
    }
}

void MainWindow::UpdateProjectionMethod(wxCommandEvent& e)
{
	const ProjectionMethod methods [] = {
		ProjectionMethod::MAX_INTENSITY, ProjectionMethod::AVERAGE, 
		ProjectionMethod::FIRST_HIT    , ProjectionMethod::COMPOSITE
	};
	m_rootCanvas3d->SetRenderMethod(methods[m_renderMethodSelect->GetSelection()],
								    (R64) m_firstHitThresholdSlider->GetValue() / 100.0);	
}

void MainWindow::InitializeVolumeModel()
{
	wxGLCanvas* canvas = new wxGLCanvas(this, wxID_ANY, nullptr);
	
	m_volumeModel->m_sharedContext = std::make_shared<wxGLContext>(canvas);
	m_volumeModel->m_dataset		  = nullptr;
	m_volumeModel->m_texture		  = {};

	canvas->SetCurrent(*m_volumeModel->m_sharedContext);

    glewExperimental = GL_TRUE;
    GLenum status    = glewInit();

    m_volumeModel->CreateTransferFuncTexture(canvas);

    //if (status != GLEW_OK)
    //{
    //    const GLubyte* msg = glewGetErrorString(status);
    //    throw std::runtime_error(std::string("GLEW Error") + std::string(reinterpret_cast<const char*>(msg)));
    //}

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

	m_renderMethodSelect->Enable();
	m_boxVisibleCheck->Enable();
	m_firstHitThresholdSlider->Enable();
	m_renderMethodSelect->Bind(wxEVT_COMBOBOX, &MainWindow::UpdateProjectionMethod, this); 
	m_firstHitThresholdSlider->Bind(wxEVT_SLIDER, &MainWindow::UpdateProjectionMethod, this);
	m_rootCanvas3d->SetBoundingBoxVisibility(m_boxVisibleCheck->IsChecked());
	m_boxVisibleCheck->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& e) {
		m_rootCanvas3d->SetBoundingBoxVisibility(m_boxVisibleCheck->IsChecked());
	});
}

void MainWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void MainWindow::OnLoadDataset(const char* path, const VolumeDataType type, 
							   const U32 width, const U32 height, const U32 depth,
							   const R64 spaceX, const R64 spaceY, const R64 spaceZ)
{
	try {
		VolumeDataset* dataset = new VolumeDataset(path, type, {width, height, depth}, {spaceX, spaceY, spaceZ});
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
						  metadataDlg->GetDepthInput(),
						  metadataDlg->GetSpaceXInput(),
						  metadataDlg->GetSpaceYInput(),
						  metadataDlg->GetSpaceZInput());
		}
	}
	fileDialog->Destroy();
	metadataDlg->Destroy();
}
