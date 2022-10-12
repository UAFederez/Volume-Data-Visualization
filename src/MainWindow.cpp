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

	m_dataset.reset(new VolumeDataset("data/PVSJ_882.raw", VolumeDataType::UINT8, {1008, 1024, 1733}));

	/**
	* The wxGLCanvas corresponding to the 3D view will represent the initial
	* starting point or root from which the OpenGL context is created and shared
	* among other wxGLCanvases
	**/
	m_rootCanvas3d  = new VolumeViewCanvas3D(this, m_dataset.get(), nullptr);
	m_sharedContext = m_rootCanvas3d->CreateContextFromThisCanvas();

	m_sagittalView   = new VolumeViewCanvas2D(this, m_dataset.get(), AnatomicalAxis::SAGITTAL, m_sharedContext);
	m_coronalView    = new VolumeViewCanvas2D(this, m_dataset.get(), AnatomicalAxis::CORONAL, m_sharedContext);
	m_horizontalView = new VolumeViewCanvas2D(this, m_dataset.get(), AnatomicalAxis::HORIZONTAL, m_sharedContext);

	// Note: refactor data sharing to be extensible other than just the texture
	m_sagittalView->SetTextureId(m_rootCanvas3d->GetTextureId());
	m_coronalView->SetTextureId(m_rootCanvas3d->GetTextureId());
	m_horizontalView->SetTextureId(m_rootCanvas3d->GetTextureId());

	wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* viewContainerSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* bottomViewsSizer   = new wxBoxSizer(wxHORIZONTAL);

	bottomViewsSizer->Add(m_sagittalView, 1, wxEXPAND);
	bottomViewsSizer->Add(m_coronalView, 1, wxEXPAND);
	bottomViewsSizer->Add(m_horizontalView, 1, wxEXPAND);
	
	viewContainerSizer->Add(m_rootCanvas3d, 1, wxEXPAND);
	viewContainerSizer->Add(bottomViewsSizer, 1, wxEXPAND);

	//rootSizer->Add(new wxPanel(this, wxID_ANY), 1, wxEXPAND);
	rootSizer->Add(viewContainerSizer, 4, wxEXPAND);

	SetSizerAndFit(rootSizer);
}

void MainWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void MainWindow::OnLoadDataset(const char* path)
{
	bool isValid = true;

	// TODO: metadata validation

	if (isValid)
	{
		m_rootCanvas3d->UpdateDataset(m_dataset.get());
		m_sagittalView->UpdateDataset(m_dataset.get());
		m_coronalView->UpdateDataset(m_dataset.get());
		m_horizontalView->UpdateDataset(m_dataset.get());
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
			OnLoadDataset(fileName.c_str());
			wxLogMessage(fileName);
		}
	}
	fileDialog->Destroy();
	metadataDlg->Destroy();
}