#pragma once

#include <glad/glad.h>
#include <cstdint>
#include <memory>

namespace vr
{
    struct TextureProperties
    {
        GLuint width       = 0;
        GLuint height      = 0;
        GLuint depth       = 0;
        GLenum target      = GL_TEXTURE_2D;
        GLenum datatype    = GL_UNSIGNED_BYTE;
        GLuint numChannels = 4; 
        GLuint minFilter   = GL_LINEAR;
        GLuint magFilter   = GL_LINEAR;
        GLuint wrappingS   = GL_CLAMP_TO_EDGE;
        GLuint wrappingT   = GL_CLAMP_TO_EDGE;
        GLuint wrappingR   = GL_CLAMP_TO_EDGE;
        GLuint fmtInternal = GL_RGBA;
        GLuint fmtSource   = GL_RGBA;
        GLfloat border[4]  = {0.0f, 0.0f, 0.0f, 0.0f};
        bool areMipmapsEnabled = true;

        bool operator==(const TextureProperties& o)
        {
            return (width      == o.width)     && (height    == o.height)    && (depth       == o.depth)       &&
                   (target     == o.target)    && (datatype  == o.datatype)  && (numChannels == o.numChannels) &&
                   (minFilter  == o.minFilter) && (magFilter == o.magFilter) && (wrappingS   == o.wrappingS)   &&
                   (wrappingT  == o.wrappingT) && (wrappingR == o.wrappingR) && (fmtInternal == o.fmtInternal) &&
                   (fmtSource  == o.fmtSource);
        }
    };

    class Texture
    {
    public:
        Texture() = default;
        Texture(const uint8_t* data, const TextureProperties& props);
        Texture(Texture&&);
        Texture& operator=(Texture&&) noexcept;

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void UpdateTexture(const uint8_t* data, const TextureProperties& props);
        
        void BindTexture() const 
        { 
            glBindTexture(m_target, m_textureId);
        }

        void UnbindTexture() const 
        { 
            glBindTexture(m_target, 0);
        }
        
        GLuint GetTextureID() const 
        { 
            return m_textureId; 
        }
        
        bool IsValid() const 
        { 
            return m_isValid;   
        }

        ~Texture();
    private:
        void Deallocate();
        GLenum m_target    = GL_TEXTURE_2D;
        GLuint m_textureId = {};
        TextureProperties  m_prevProps = {};
        bool   m_isValid   = false;
    };
}
