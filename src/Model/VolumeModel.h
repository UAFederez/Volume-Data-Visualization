#pragma once

#include "../Graphics/Texture.h"
#include "VolumeDataset.h"
#include "TransferFunction1D.h"

#include <vector>
#include <string>
#include <unordered_map>

namespace vr
{
    class VolumeModel
    {
        public:
            VolumeModel() = default;
            VolumeModel(VolumeDataset&& dataset, Texture&& texture)             
            {
                m_dataset   = std::move(dataset);
                m_texture3d = std::move(texture);
            }

            VolumeModel(VolumeModel&&)            = default;
            VolumeModel& operator=(VolumeModel&&) = default;
            
            VolumeModel(const VolumeModel&)            = delete;
            VolumeModel& operator=(const VolumeModel&) = delete;

            R64 GetMaxInDoubleRange() const
            {
                return m_dataset.GetMaxInDoubleRange();
            }

            R64 GetMinInDoubleRange() const
            {
                return m_dataset.GetMinInDoubleRange();
            }

            auto GetDataSpacing() const
            {
                return m_dataset.DataSpacing();
            }

            auto GetDataSize() const
            {
                return m_dataset.DataSize();
            }

            GLuint GetTextureID() const
            {
                return m_texture3d.GetTextureID();
            }

            void BindTexture() const
            {
                m_texture3d.BindTexture();
            }

            void UnbindTexture() const
            {
                m_texture3d.BindTexture();
            }

            GLuint GetTransferTextureID(const std::string& name) const
            {
                return m_transferFunctions.at(name).m_cmapTex;
            }

            void AddTransferFunction(const std::string& name, const TransferFunction1D& f)
            {
                m_transferFunctions[name] = f;
            }
        private:
            Texture m_texture3d;
            VolumeDataset m_dataset;
            std::unordered_map<std::string, TransferFunction1D> m_transferFunctions;
    };
}
