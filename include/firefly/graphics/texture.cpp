#include <firefly/graphics/texture.hpp>
#include <firefly/debug/log.hpp>
#include <firefly/debug/gl_debug.hpp>
#include <firefly/io/SOIL/SOIL.h>

////////////////////////////////////////////////////////////////////////

namespace ff {

// create global instance

    TextureMgr GlobalTextureMgr;


// queries hardware vendor for texture support

	void TextureMgr::Init() 
	{
		GLfloat maxAnisotropy = 0.f;
		if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
			GL_DEBUG(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy));
			g_Log.write(LOG_CONFIG, "TextureMgr > %.0fx Anistropic filtering supported.", maxAnisotropy);
		} else {
			g_Log.write(LOG_CONFIG, "TextureMgr > Anistropic filtering NOT supported.");
		}

		// use max anisotropic filtering by default
		afLevel = afMax = (GLint) maxAnisotropy;
	}


// loads an image file into OpenGL using SOIL

	GLuint TextureMgr::LoadTexture(string file, bool repeats, bool compress)
	{
		// extende filename
		file = FF_TEXTURE_PATH + file;

		uint32 tFlags = SOIL_FLAG_MIPMAPS;
		if (repeats)  tFlags |= SOIL_FLAG_TEXTURE_REPEATS;
		if (compress) tFlags |= SOIL_FLAG_COMPRESS_TO_DXT;

		// create texture using SOIL
		GLuint handle = SOIL_load_OGL_texture(file.c_str(), 0, 0, tFlags);

		// set texture parameters (fixed at max quality currently)
		GL_DEBUG(glBindTexture(GL_TEXTURE_2D, handle));
		GL_DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GL_DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, afLevel));

		// store handle
		m_textures.push_back(handle);
		return handle;
	}


// deletes all loaded textures

	void TextureMgr::DeleteTextures()
	{
		if (m_textures.empty())
            return;

        GL_DEBUG(glDeleteTextures(m_textures.size(), &m_textures[0]));
        m_textures.clear();
	}


// sets anisotropic filtering level for all textures (current and future)

	void TextureMgr::SetAnisotropic(GLint level) 
	{
		afLevel = (level >= 0 && level <= afMax) ? level : afMax;
		for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
		{
			GL_DEBUG(glBindTexture(GL_TEXTURE_2D, *it));
			GL_DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, afLevel));
		}
	}

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

