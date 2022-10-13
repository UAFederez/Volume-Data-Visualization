#pragma once

#include <wx/wx.h>

#include <wx/spinctrl.h>

#include "VolumeDataset.h"

class VolumeMetadataDialog : public wxDialog
{
public:
	VolumeMetadataDialog(const wxString& title);


	U32 GetDepthInput()  const { return m_depthInput->GetValue(); }
	U32 GetWidthInput()  const { return m_widthInput->GetValue(); }
	U32 GetHeightInput() const { return m_heightInput->GetValue(); }
private:
	VolumeDataType type	    = VolumeDataType::UINT8;
	std::array<U32, 3> size = { 0 };

	wxSpinCtrl* m_depthInput  = nullptr;
	wxSpinCtrl* m_widthInput  = nullptr;
	wxSpinCtrl* m_heightInput = nullptr;

};