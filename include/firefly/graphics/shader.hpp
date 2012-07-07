#ifndef FIREFLY_SHADER_HPP
#define FIREFLY_SHADER_HPP

#include <firefly/opengl.hpp>
#include <firefly/common.hpp>
#include <firefly/core/singleton.hpp>

#define FF_SHADER_PATH "data/shader/"

////////////////////////////////////////////////////////////////////////

namespace ff {

// basic shader info

    struct Shader
    {
        GLuint programID;
        GLuint vertID;
        GLuint fragID;
    };

// shader manager singleton

    class ShaderMgr : public singleton<ShaderMgr>
    {
    public:
        ShaderMgr() : m_cur(0) { }
        ~ShaderMgr() { DeletePrograms(); }

        void DeletePrograms();
        bool Compile(string name, string vert, string frag);
        bool Compile(string name, string prefix);
        bool Compile(string name);

        uint32 Get(string program = "");
        uint32 Use(string program) {m_cur = Get(program); return m_cur;}

    protected:
        map<string, Shader> m_programs;
        GLuint m_cur;
    };

// global access

    extern ShaderMgr GlobalShaderMgr;

#define g_Shader ff::ShaderMgr::get_singleton()

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
