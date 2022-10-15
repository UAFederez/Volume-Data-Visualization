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

class VolumeDataset
{
	public:
		VolumeDataset(const std::string& path, 
					  const VolumeDataType type,
					  const std::array<U32, 3> size,
					  const std::array<R64, 3> spacing);
		
		VolumeDataset(const VolumeDataset&) = delete;
		VolumeDataset(VolumeDataset&&)		= delete;
		VolumeDataset& operator=(const VolumeDataset&) = delete;
		VolumeDataset& operator=(VolumeDataset&&)	   = delete;

		auto DataSize() const 
		{ 
			return m_dataSize; 
		}

		auto DataSpacing() const
		{
			return m_spacing;
		}
		
		auto DataType() const 
		{ 
			return m_dataType; 
		}
		
		const U8* RawData() const 
		{ 
			return m_rawBytes.data(); 
		}

		const R64 GetMaxInDoubleRange() const
		{
			return m_maxInDouble;
		}
	private:
		void ReadVolumeDatasetFile(const std::string& path);
		
		template <typename T>
		void ComputeStatistics();

		R64 m_maxInDouble = 1.0f;
		R64 m_minInDouble = 0.0f;

		std::string		   m_filePath;
		std::vector<U8>    m_rawBytes;
		std::array<R64, 3> m_spacing    = {0};
		std::array<R32, 3> m_actualSize = {0};
		std::array<U32, 3> m_dataSize   = {0};
		VolumeDataType     m_dataType;
};