#include "VolumeMetadataDialog.h"

VolumeMetadataDialog::VolumeMetadataDialog(const wxString& title)
	: wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(320, 320))
{
	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);

	// Width input
	m_widthInput = new wxSpinCtrl(this, wxID_ANY);
	m_widthInput->SetMin(0);
	m_widthInput->SetMax(std::numeric_limits<I32>::max());
	wxBoxSizer* widthInputSizer = new wxBoxSizer(wxHORIZONTAL);

	widthInputSizer->Add(new wxStaticText(this, wxID_ANY, "Width:"), 3, wxEXPAND);
	widthInputSizer->Add(m_widthInput, 9, wxEXPAND);

	// Height input
	m_heightInput = new wxSpinCtrl(this, wxID_ANY);
	m_heightInput->SetMin(0);
	m_heightInput->SetMax(std::numeric_limits<I32>::max());
	wxBoxSizer* heightInputSizer = new wxBoxSizer(wxHORIZONTAL);

	heightInputSizer->Add(new wxStaticText(this, wxID_ANY, "Height:"), 3, wxEXPAND);
	heightInputSizer->Add(m_heightInput, 9, wxEXPAND);

	// Depth input
	m_depthInput = new wxSpinCtrl(this, wxID_ANY);
	m_depthInput->SetMin(0);
	m_depthInput->SetMax(std::numeric_limits<I32>::max());
	wxBoxSizer* depthInputSizer = new wxBoxSizer(wxHORIZONTAL);

	depthInputSizer->Add(new wxStaticText(this, wxID_ANY, "Depth:"), 3, wxEXPAND);
	depthInputSizer->Add(m_depthInput, 9, wxEXPAND);

	// Data type input
	wxBoxSizer* typeInputSizer = new wxBoxSizer(wxHORIZONTAL);

	wxArrayString typeOptions = wxArrayString();
	typeOptions.Add(wxString("8-bit unsigned integer"));

	wxComboBox* typeSelect = new wxComboBox(this, wxID_ANY, "8-bit signed integer", wxPoint(0, 0), wxSize(256, 32), typeOptions);
	typeSelect->Disable();

	typeInputSizer->Add(new wxStaticText(this, wxID_ANY, "Data type:"), 3, wxEXPAND);
	typeInputSizer->Add(typeSelect, 9, wxEXPAND);

	// Submit and cancel buttons Button 
	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->Add(new wxButton(this, wxID_OK, wxString("Ok")), 1, wxEXPAND);
	buttonSizer->Add(new wxButton(this, wxID_CANCEL, wxString("Cancel")), 1, wxEXPAND);

	rootSizer->Add(widthInputSizer  , 1 , wxEXPAND | wxALL , 5);
	rootSizer->Add(heightInputSizer , 1 , wxEXPAND | wxALL , 5);
	rootSizer->Add(depthInputSizer  , 1 , wxEXPAND | wxALL , 5);
	rootSizer->Add(typeInputSizer   , 1 , wxEXPAND | wxALL , 5);
	rootSizer->Add(buttonSizer      , 1 , wxEXPAND | wxALL , 5);
	
	SetSizerAndFit(rootSizer);
	Centre();
}
