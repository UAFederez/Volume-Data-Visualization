#include "VolumeDataset.h"

#include <fstream>
#include <filesystem>
#include <iostream>
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
		throw std::exception(std::string(std::string("File not found: ") + path).c_str());

	std::uintmax_t size = std::filesystem::file_size(path);
	const U64 pointSize = static_cast<std::underlying_type<VolumeDataType>::type>(dataType);
	const U64 totalSize = (U64) dataSize[0] * dataSize[1] * dataSize[2] * pointSize;

	if (size != totalSize)
		throw std::exception("Invalid size specified");

	rawBytes.resize(totalSize);
	inputFile.read(reinterpret_cast<char*>(rawBytes.data()), totalSize);
}