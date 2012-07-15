#ifndef FIREFLY_TEXTURE_HPP
#define FIREFLY_TEXTURE_HPP

#include <firefly/opengl.hpp>
#include <firefly/common.hpp>
#include <firefly/core/singleton.hpp>

#define FF_TEXTURE_PATH "data/texture/"
#define FF_MAX_ANISOTROPY 16

typedef enum FF_TEXTURE_TYPE {
	FF_TEXTURE_TGA,
	FF_TEXTURE_JPG,
	FF_TEXTURE_PNG,
	FF_TEXTURE_DDS,
};

/*
	Model {
		Mesh
		Material
	}
	Material {
		Texture
		Shader
	}
	Mesh {
		IBO
		VBO
	}
 */

////////////////////////////////////////////////////////////////////////

namespace ff {

// texture manager singleton

    class TextureMgr : public singleton<TextureMgr>
    {
    public:
        TextureMgr() : afLevel(0), afMax(0) { }
        ~TextureMgr() { }
      
		// query vendor for texture support
		void Init();

		// load / delete
		GLuint LoadTexture(string file, bool repeats = false, bool compress = true);
		void DeleteTextures();

		// config parameters
		void SetAnisotropic(GLint level = FF_MAX_ANISOTROPY);

    protected:
        vector<GLuint> m_textures;
		GLint afLevel;
		GLint afMax;
    };

// global access

    extern TextureMgr GlobalTextureMgr;

#define g_Texture ff::TextureMgr::get_singleton()

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
