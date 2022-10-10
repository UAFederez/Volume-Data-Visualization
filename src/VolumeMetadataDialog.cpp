#include "VolumeMetadataDialog.h"

VolumeMetadataDialog::VolumeMetadataDialog(const wxString& title)
	: wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(320, 320))
{
	wxPanel* panel = new wxPanel(this);

	wxArrayString typeOptions = wxArrayString();
	typeOptions.Add(wxString("8-bit signed integer"));
	typeOptions.Add(wxString("8-bit unsigned integer"));
	typeOptions.Add(wxString("16-bit signed integer"));
	typeOptions.Add(wxString("16-bit unsigned integer"));
	typeOptions.Add(wxString("32-bit signed integer"));
	typeOptions.Add(wxString("32-bit unsigned integer"));
	typeOptions.Add(wxString("64-bit signed integer"));
	typeOptions.Add(wxString("64-bit unsigned integer"));

	wxBoxSizer* vertBox    = new wxBoxSizer(wxVERTICAL);
	wxComboBox* typeSelect = new wxComboBox(panel, wxID_ANY, "8-bit signed integer", wxPoint(0, 0), wxSize(256, 32), typeOptions);

	vertBox->Add(panel, 0, wxALL, 10);

	SetSizer(vertBox);

	Centre();
	ShowModal();

	Destroy();
}