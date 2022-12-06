#include "Texture.h"

namespace vr
{
    Texture::Texture(const uint8_t* data, const TextureProperties& props)
    {
        UpdateTexture(data, props);
    }

    void Texture::UpdateTexture(const uint8_t* data, const TextureProperties& props)
    {
        Deallocate();

        const bool isTexture3d = props.target == GL_TEXTURE_3D;

        glGenTextures(1, &m_textureId);
        glBindTexture(props.target, m_textureId);

        glTexParameteri(props.target, GL_TEXTURE_WRAP_S, props.wrappingS);
        glTexParameteri(props.target, GL_TEXTURE_WRAP_T, props.wrappingT);
        glTexParameteri(props.target, GL_TEXTURE_MIN_FILTER, props.minFilter);
        glTexParameteri(props.target, GL_TEXTURE_MAG_FILTER, props.magFilter);

        if ( props.wrappingS == GL_CLAMP_TO_BORDER ||
             props.wrappingT == GL_CLAMP_TO_BORDER ||
            (props.wrappingR == GL_CLAMP_TO_BORDER && isTexture3d))
        {
            glTexParameterfv(props.target, GL_TEXTURE_BORDER_COLOR, props.border);
        }

        if(props.target == GL_TEXTURE_3D)
            glTexParameteri(props.target, GL_TEXTURE_WRAP_R, props.wrappingR);
        if (isTexture3d)
        {
            glTexImage3D(props.target, 0, props.fmtInternal,
                props.width, props.height, props.depth,
                0, props.fmtSource, props.datatype, data);
        }
        else
        {
            glTexImage2D(props.target,  0, props.fmtInternal, 
                props.width, props.height, 0,
                props.fmtSource, props.datatype, data);
        }

        if (props.areMipmapsEnabled)
            glGenerateMipmap(props.target);

        m_target = props.target;

        glBindTexture(props.target, 0);
        m_isValid = true;

        m_prevProps = props;
    }

    Texture::Texture(Texture&& other):
        m_target   (std::move(other.m_target   )),
        m_textureId(std::move(other.m_textureId)),
        m_prevProps(std::move(other.m_prevProps)),
        m_isValid  (std::move(other.m_isValid  ))
    {
        other.m_isValid   = false;
        other.m_prevProps = {};
        other.m_textureId = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        Deallocate();

        m_target    = std::move(other.m_target);
        m_textureId = std::move(other.m_textureId);
        m_prevProps = std::move(other.m_prevProps);
        m_isValid   = std::move(other.m_isValid);

        other.m_isValid   = false;
        other.m_prevProps = {};
        other.m_textureId = 0;

        return *this;
    }

    Texture::~Texture()
    {
        Deallocate();
    }
    
    void Texture::Deallocate()
    {
        if (m_isValid)
        {
            glDeleteTextures(1, &m_textureId);
            m_isValid = false;
        }
    }
}
