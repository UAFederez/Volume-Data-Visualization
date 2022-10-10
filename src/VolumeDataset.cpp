#include "VolumeDataset.h"

#include <fstream>
#include <thread>

VolumeDataset::VolumeDataset(const std::string& path, 
							 const VolumeDataType type,
							 const std::array<U32, 3> size):
	filePath(path),
	dataType(type),
	dataSize(size)
{
	ReadVolumeDatasetFile(filePath);
}

void VolumeDataset::ReadVolumeDatasetFile(const std::string& path)
{
	std::ifstream inputFile(path, std::ios::binary);
	if(!inputFile)
		throw std::runtime_error("File not found: " + path);

	const U64 totalSize = (U64) dataSize[0] * dataSize[1] * dataSize[2];

	rawBytes.resize(totalSize);
	inputFile.read(reinterpret_cast<char*>(rawBytes.data()), totalSize);
}