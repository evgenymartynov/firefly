#ifndef FIREFLY_SHADER_HPP
#define FIREFLY_SHADER_HPP

#include <firefly/opengl.hpp>
#include <firefly/common.hpp>
#include <firefly/core/singleton.hpp>

#define FF_SHADER_PATH "data/shader/"

////////////////////////////////////////////////////////////////////////

namespace ff {

// shader manager singleton

    class ShaderMgr : public singleton<ShaderMgr>
    {
    public:
        ShaderMgr() { }
        ~ShaderMgr() { }
      
		GLuint CreateProgram(string vert, string frag, ...);
		void DeletePrograms();

    protected:
        vector<GLuint> m_shaders;

		bool LoadShader(GLuint shader, string filename);
		bool CheckShaderCompile(GLint shader, const string & file);
		bool CheckProgramLink(GLint program);
    };

// global access

    extern ShaderMgr GlobalShaderMgr;

#define g_Shader ff::ShaderMgr::get_singleton()

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
