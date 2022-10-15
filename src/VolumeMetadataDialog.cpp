#include "VolumeMetadataDialog.h"

VolumeMetadataDialog::VolumeMetadataDialog(const wxString& title)
	: wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* dimensionInputSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* spacingInputSizer   = new wxBoxSizer(wxVERTICAL);

	{
		// Width input
		m_widthInput = new wxSpinCtrl(this, wxID_ANY);
		m_widthInput->SetMin(0);
		m_widthInput->SetMax(std::numeric_limits<I32>::max());
		wxBoxSizer* widthInputSizer = new wxBoxSizer(wxHORIZONTAL);

		widthInputSizer->Add(new wxStaticText(this, wxID_ANY, "Width:"), 2,  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT);
		widthInputSizer->Add(m_widthInput, 8, wxEXPAND);

		// Height input
		m_heightInput = new wxSpinCtrl(this, wxID_ANY);
		m_heightInput->SetMin(0);
		m_heightInput->SetMax(std::numeric_limits<I32>::max());
		wxBoxSizer* heightInputSizer = new wxBoxSizer(wxHORIZONTAL);

		heightInputSizer->Add(new wxStaticText(this, wxID_ANY, "Height:"), 2,  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT);
		heightInputSizer->Add(m_heightInput, 8, wxEXPAND);

		// Depth input
		m_depthInput = new wxSpinCtrl(this, wxID_ANY);
		m_depthInput->SetMin(0);
		m_depthInput->SetMax(std::numeric_limits<I32>::max());
		wxBoxSizer* depthInputSizer = new wxBoxSizer(wxHORIZONTAL);

		depthInputSizer->Add(new wxStaticText(this, wxID_ANY, "Depth:"), 2,  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT);
		depthInputSizer->Add(m_depthInput, 8, wxEXPAND);

		dimensionInputSizer->Add(widthInputSizer, 1, wxEXPAND | wxTOP | wxBOTTOM, 5);
		dimensionInputSizer->Add(heightInputSizer, 1, wxEXPAND | wxTOP | wxBOTTOM, 5);
		dimensionInputSizer->Add(depthInputSizer, 1, wxEXPAND | wxTOP | wxBOTTOM, 5);
	}
	
	{
		// Spacing X input
		m_spaceXInput = new wxSpinCtrlDouble(this, wxID_ANY);
		m_spaceXInput->SetDigits(6);
		wxBoxSizer* spaceXInputSizer = new wxBoxSizer(wxHORIZONTAL);

		spaceXInputSizer->Add(new wxStaticText(this, wxID_ANY, "Spacing X:"), 2,  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT);
		spaceXInputSizer->Add(m_spaceXInput, 8, wxEXPAND, 0);

		m_spaceYInput = new wxSpinCtrlDouble(this, wxID_ANY);
		m_spaceYInput->SetDigits(6);
		wxBoxSizer* spaceYInputSizer = new wxBoxSizer(wxHORIZONTAL);

		spaceYInputSizer->Add(new wxStaticText(this, wxID_ANY, "Spacing Y:"), 2,  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT);
		spaceYInputSizer->Add(m_spaceYInput, 8, wxEXPAND, 0);

		m_spaceZInput = new wxSpinCtrlDouble(this, wxID_ANY);
		m_spaceZInput->SetDigits(6);
		wxBoxSizer* spaceZInputSizer = new wxBoxSizer(wxHORIZONTAL);

		spaceZInputSizer->Add(new wxStaticText(this, wxID_ANY, "Spacing Z:"), 2,  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT);
		spaceZInputSizer->Add(m_spaceZInput, 8, wxEXPAND, 0);

		spacingInputSizer->Add(spaceXInputSizer, 1, wxEXPAND | wxTOP | wxBOTTOM, 5);
		spacingInputSizer->Add(spaceYInputSizer, 1, wxEXPAND | wxTOP | wxBOTTOM, 5);
		spacingInputSizer->Add(spaceZInputSizer, 1, wxEXPAND | wxTOP | wxBOTTOM, 5);
	}

	// Data type input
	wxBoxSizer* typeInputSizer = new wxBoxSizer(wxHORIZONTAL);

	wxArrayString typeOptions = wxArrayString();
	typeOptions.Add(wxString("8-bit unsigned integer"));
	typeOptions.Add(wxString("16-bit unsigned integer"));
	typeOptions.Add(wxString("float  (IEEE-754)"));

	m_typeSelect = new wxComboBox(this, wxID_ANY, "8-bit unsigned integer", wxPoint(0, 0), wxSize(256, 32), typeOptions, wxCB_READONLY);

	typeInputSizer->Add(new wxStaticText(this, wxID_ANY, "Data type:"), 2);
	typeInputSizer->Add(m_typeSelect, 8);

	m_okButton = new wxButton(this, wxID_OK, wxString("Ok"));
	m_okButton->Disable();

	// Submit and cancel buttons Button 
	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->Add(m_okButton, 1);
	buttonSizer->Add(new wxButton(this, wxID_CANCEL, wxString("Cancel")), 1);

	rootSizer->Add(dimensionInputSizer, 0 , wxEXPAND | wxTOP | wxBOTTOM, 2);
	rootSizer->Add(spacingInputSizer, 0 , wxEXPAND | wxTOP | wxBOTTOM, 2);
	rootSizer->Add(typeInputSizer, 0 , wxEXPAND | wxTOP | wxBOTTOM, 2);
	rootSizer->Add(buttonSizer, 0 , wxEXPAND | wxTOP | wxBOTTOM, 2);
	
	wxBoxSizer* mainContainerSizer = new wxBoxSizer(wxVERTICAL);
	mainContainerSizer->Add(rootSizer, 1, wxEXPAND | wxALL, 15);

	m_widthInput->Bind(wxEVT_SPINCTRL, &VolumeMetadataDialog::ValidateIntInput, this);
	m_heightInput->Bind(wxEVT_SPINCTRL, &VolumeMetadataDialog::ValidateIntInput, this);
	m_depthInput->Bind(wxEVT_SPINCTRL, &VolumeMetadataDialog::ValidateIntInput, this);

	m_spaceXInput->Bind(wxEVT_SPINCTRLDOUBLE, &VolumeMetadataDialog::ValidateDoubleInput, this);
	m_spaceYInput->Bind(wxEVT_SPINCTRLDOUBLE, &VolumeMetadataDialog::ValidateDoubleInput, this);
	m_spaceZInput->Bind(wxEVT_SPINCTRLDOUBLE, &VolumeMetadataDialog::ValidateDoubleInput, this);

	SetSizerAndFit(mainContainerSizer);
	Centre();
}

void VolumeMetadataDialog::ValidateInput()
{
	bool isValidWidth  = m_widthInput->GetValue() > 0;
	bool isValidHeight = m_heightInput->GetValue() > 0;
	bool isValidDepth  = m_depthInput->GetValue() > 0;

	bool isValidSpaceX = m_spaceXInput->GetValue() > 0.0f;
	bool isValidSpaceY = m_spaceYInput->GetValue() > 0.0f;
	bool isValidSpaceZ = m_spaceZInput->GetValue() > 0.0f;

	if (isValidWidth && isValidHeight && isValidDepth &&
		isValidSpaceX && isValidSpaceY && isValidSpaceZ)
	{
		m_okButton->Enable();
	} else
		m_okButton->Disable();

}

void VolumeMetadataDialog::ValidateIntInput(wxSpinEvent& e)
{
	ValidateInput();
}

void VolumeMetadataDialog::ValidateDoubleInput(wxSpinDoubleEvent& e)
{
	ValidateInput();
}

VolumeDataType VolumeMetadataDialog::GetDataType() const
{
	static const VolumeDataType dataTypes[] = {
		VolumeDataType::UINT8, VolumeDataType::UINT16, VolumeDataType::FLOAT,
	};
	return dataTypes[m_typeSelect->GetSelection()];
}
