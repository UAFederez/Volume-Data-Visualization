#pragma once

#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "VolumeModel/VolumeDataset.h"

class VolumeMetadataDialog : public wxDialog
{
public:
	VolumeMetadataDialog(const wxString& title);


	U32 GetDepthInput()  const { return m_depthInput->GetValue() ; }
	U32 GetWidthInput()  const { return m_widthInput->GetValue() ; }
	U32 GetHeightInput() const { return m_heightInput->GetValue(); }

	R32 GetSpaceXInput() const { return m_spaceXInput->GetValue(); }
	R32 GetSpaceYInput() const { return m_spaceYInput->GetValue(); }
	R32 GetSpaceZInput() const { return m_spaceZInput->GetValue(); }

	VolumeDataType GetDataType() const;
private:
	void ValidateInput();
	void ValidateIntInput(wxSpinEvent& e);
	void ValidateDoubleInput(wxSpinDoubleEvent& e);


	VolumeDataType type	    = VolumeDataType::UINT8;
	std::array<U32, 3> size = { 0 };

	wxButton*   m_okButton    = nullptr;
	wxSpinCtrl* m_depthInput  = nullptr;
	wxSpinCtrl* m_widthInput  = nullptr;
	wxSpinCtrl* m_heightInput = nullptr;
	wxSpinCtrlDouble* m_spaceXInput = nullptr;
	wxSpinCtrlDouble* m_spaceYInput = nullptr;
	wxSpinCtrlDouble* m_spaceZInput = nullptr;
	wxComboBox* m_typeSelect  = nullptr;
};