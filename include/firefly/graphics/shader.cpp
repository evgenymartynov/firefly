#include <firefly/graphics/shader.hpp>
#include <firefly/debug/gl_debug.hpp>
#include <firefly/debug/log.hpp>
#include <fstream>

using std::ifstream;
using std::ios;

////////////////////////////////////////////////////////////////////////

namespace ff {

// create global instance

    ShaderMgr GlobalShaderMgr;


// checks for any shader compilation errors

    bool debug_shader_compile(GLint shader, const string & file)
    {
        vector<char> err;
        GLint success = GL_FALSE;
        int len;

        GL_DEBUG(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
        if (!success)
        {
            GL_DEBUG(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len));
            err.resize(len);
            GL_DEBUG(glGetShaderInfoLog(shader, len, NULL, &err[0]));
            g_Log.write(LOG_ERROR, "ShaderMgr::Compile > Failed to "
                        "compile '%s'!", file.c_str());
            g_Log.write(LOG_ERROR, &err[0]);
            return false;
        }
        return true;
    }


// checks for any program compilation errors

    bool debug_program_compile(GLint program, const string & name)
    {
        vector<char> err;
        GLint success = GL_FALSE;
        int len;

        GL_DEBUG(glGetProgramiv(program, GL_LINK_STATUS, &success));
        if (!success)
        {
            GL_DEBUG(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len));
            err.resize(len);
            GL_DEBUG(glGetProgramInfoLog(program, len, NULL, &err[0]));
            g_Log.write(LOG_ERROR, "ShaderMgr::Compile > Failed to "
                        "compile program '%s'!", name.c_str());
            g_Log.write(LOG_ERROR, &err[0]);
            return false;
        }
        return true;
    }

// deletes all loaded programs

    void ShaderMgr::DeletePrograms()
    {
        if (m_programs.empty())
            return;

        auto it = m_programs.begin(), end = m_programs.end();
        for (; it != end; ++it)
        {
            Shader & s = it->second;

            GL_DEBUG(glUseProgram(0));
            GL_DEBUG(glDetachShader(s.programID, s.vertID));
            GL_DEBUG(glDetachShader(s.programID, s.fragID));
            GL_DEBUG(glDeleteShader(s.vertID));
            GL_DEBUG(glDeleteShader(s.fragID));
            GL_DEBUG(glDeleteProgram(s.programID));
        }

        m_programs.clear();
        m_cur = 0;
    }

// compiles a single shader using its name as the prefix

    bool ShaderMgr::Compile(string name)
    {
        return Compile(name, name + ".vert", name + ".frag");
    }


// compiles a single shader given a common shader prefix

    bool ShaderMgr::Compile(string name, string prefix)
    {
        return Compile(name, prefix + ".vert", prefix + ".frag");
    }


// compiles a single shader with explicitly specified vert/frag files

    bool ShaderMgr::Compile(string name, string vert, string frag)
    {
        Shader s;
        vector<char> vertData, fragData;
        GLint vertSize = 0;
        GLint fragSize = 0;

        // load shader strings from files
        string path = FF_SHADER_PATH;
        ifstream in_vert((path + vert).c_str(), ios::in | ios::binary);
        ifstream in_frag((path + frag).c_str(), ios::in | ios::binary);

        if (!in_vert.is_open() || !in_frag.is_open())
        {
            g_Log.write(LOG_ERROR, "ShaderMgr::Compile > unable to"
                  " find files %s, %s!",vert.c_str(), frag.c_str());
            return false;
        }

        in_vert.seekg(0, ios::end);
        vertSize = (int)in_vert.tellg();
        vertData.resize(vertSize);
        in_vert.seekg(0, ios::beg);

        in_frag.seekg(0, ios::end);
        fragSize = (int)in_frag.tellg();
        fragData.resize(fragSize);
        in_frag.seekg(0, ios::beg);

        in_vert.read(&vertData[0], vertSize);
        in_vert.close();
        in_frag.read(&fragData[0], fragSize);
        in_frag.close();

        s.vertID = GL_CHECK(glCreateShader(GL_VERTEX_SHADER));
        s.fragID = GL_CHECK(glCreateShader(GL_FRAGMENT_SHADER));

        // compile shaders
        const GLchar * vertSrc = &vertData[0];
        GL_DEBUG(glShaderSource(s.vertID, 1, &vertSrc, &vertSize));
        GL_DEBUG(glCompileShader(s.vertID));
        debug_shader_compile(s.vertID, vert);

        const GLchar * fragSrc = &fragData[0];
        GL_DEBUG(glShaderSource(s.fragID, 1, &fragSrc, &fragSize));
        GL_DEBUG(glCompileShader(s.fragID));
        debug_shader_compile(s.fragID, frag);

        // create shader program
        s.programID = GL_CHECK(glCreateProgram());
        GL_DEBUG(glAttachShader(s.programID, s.vertID));
        GL_DEBUG(glAttachShader(s.programID, s.fragID));
        GL_DEBUG(glLinkProgram (s.programID));

        if (debug_program_compile(s.programID, name))
        {
            m_cur = s.programID;
            GL_DEBUG(glBindAttribLocation(m_cur, 0, "in_Position"));
            GL_DEBUG(glBindAttribLocation(m_cur, 1, "in_UV"));
            m_programs[name] = s;
            g_Log.write(LOG_EVENT, "Compiled '%s' shader program.",
                        name.c_str());
            return true;
        }
        else
        {
            return false;
        }
    }


// returns the program id for requested shader

    uint32 ShaderMgr::Get(string program)
    {
        if (program.empty())
            return 0;

        auto p = m_programs.find(program);

        if (p == m_programs.end())
            return 0;

        return (p->second.programID);
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
