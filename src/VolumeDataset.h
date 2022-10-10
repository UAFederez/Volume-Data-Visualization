#pragma once

#include <vector>
#include <array>
#include <string>

using R32 = float;
using R64 = double;
using U64 = uint64_t;
using U32 = uint32_t;
using U16 = uint16_t;
using U8  = uint8_t;
using I64 = int64_t;
using I32 = int32_t;
using I16 = int16_t;
using I8  = int8_t;

enum class VolumeDataType { 
	INT8  = 1, INT16  = 2, INT32  = 4, INT64  = 8, 
	UINT8 = 1, UINT16 = 2, UINT32 = 4, UINT64 = 8, 
	FLOAT = 4, DOUBLE = 8
};

struct Colormap
{
	// any value from previous right endpoint (or 0 if this is the first)
	// will map to the corresponding color with the same index

	std::vector<U64> rightEndpoints;	// 
	std::vector<U32> colors;
};

class VolumeDataset
{
	public:
		VolumeDataset(const std::string& path, 
					  const VolumeDataType type,
					  const std::array<U32, 3> size);
		
		VolumeDataset(const VolumeDataset&) = delete;
		VolumeDataset(VolumeDataset&&)		= delete;
		VolumeDataset& operator=(const VolumeDataset&) = delete;
		VolumeDataset& operator=(VolumeDataset&&)	   = delete;

		auto DataSize() const 
		{ 
			return dataSize; 
		}
		
		auto DataType() const 
		{ 
			return dataType; 
		}
		
		const U8* RawData() const 
		{ 
			return rawBytes.data(); 
		}
	private:
		void ReadVolumeDatasetFile(const std::string& path);

		std::string		   filePath;
		std::vector<U8>    rawBytes;
		std::array<R32, 3> actualSize = {0};
		std::array<U32, 3> dataSize   = {0};
		VolumeDataType     dataType;
};