#pragma once

#include <wx/wx.h>
#include "VolumeDataset.h"

class VolumeMetadataDialog : public wxDialog
{
public:
	VolumeMetadataDialog(const wxString& title);

private:
	VolumeDataType type	    = VolumeDataType::UINT8;
	std::array<U32, 3> size = { 0 };
};