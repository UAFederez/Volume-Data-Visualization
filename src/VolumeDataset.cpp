#include "VolumeDataset.h"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <thread>

VolumeDataset::VolumeDataset(const std::string& path, 
							 const VolumeDataType type,
							 const std::array<U32, 3> size,
							 const std::array<R64, 3> spacing):
	m_filePath(path),
	m_dataType(type),
	m_dataSize(size),
	m_spacing(spacing)
{
	ReadVolumeDatasetFile(m_filePath);
}

template <typename T>
void VolumeDataset::ComputeStatistics()
{
	const U64 pointSize = sizeof(T);
	
	T min_value = *reinterpret_cast<T*>(&m_rawBytes[0]);
	T max_value = *reinterpret_cast<T*>(&m_rawBytes[pointSize]);

	const auto compute = [&](const T& val) {
		min_value = std::min<T>(val, min_value);
		max_value = std::max<T>(val, max_value);
	};

	std::for_each(reinterpret_cast<T*>(&m_rawBytes[0]),
				  reinterpret_cast<T*>(&m_rawBytes[m_rawBytes.size() - pointSize]),
				  compute);

	const auto range = std::numeric_limits<T>::max() - std::numeric_limits<T>::min();

	m_maxInDouble = (R64) max_value / (R64) range;
	m_minInDouble = (R64) min_value / (R64) range;
}

void VolumeDataset::ReadVolumeDatasetFile(const std::string& path)
{
	std::ifstream inputFile(path, std::ios::binary);
	if(!inputFile)
		throw std::exception(std::string(std::string("File not found: ") + path).c_str());

	std::uintmax_t size = std::filesystem::file_size(path);
	const U64 pointSize = static_cast<std::underlying_type<VolumeDataType>::type>(m_dataType);
	const U64 totalSize = (U64) m_dataSize[0] * m_dataSize[1] * m_dataSize[2] * pointSize;

	if (size != totalSize)
		throw std::exception("Invalid size specified");

	m_rawBytes.resize(totalSize);
	inputFile.read(reinterpret_cast<char*>(m_rawBytes.data()), totalSize);

	switch (m_dataType)
	{
		case VolumeDataType::UINT8 : ComputeStatistics<U8>() ; break;
		case VolumeDataType::UINT16: ComputeStatistics<U16>(); break;
		case VolumeDataType::FLOAT : ComputeStatistics<R32>(); break;
	}

}