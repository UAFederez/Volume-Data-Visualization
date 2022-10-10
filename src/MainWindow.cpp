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

	dataset.reset(new VolumeDataset("data/PVSJ_882.raw",
									VolumeDataType::UINT8,
									{1008, 1024, 1733}));
	m_volumeView3d = new VolumeView3D(this, dataset.get());

	Fit();
	Centre();
}

void MainWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void MainWindow::OnLoadDataset(const char* path)
{
	m_volumeView3d->UpdateDataset(dataset.get());
}

void MainWindow::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog* fileDialog = new wxFileDialog(this);
	if (fileDialog->ShowModal() == wxID_OK) 
	{
		wxString fileName = fileDialog->GetPath();
		OnLoadDataset(fileName.c_str());
		// VolumeMetadataDialog* metadataDlg = new VolumeMetadataDialog(wxString("Please specify the metadata"));
		// metadataDlg->Show(true);
		wxLogMessage(fileName);
	}
}