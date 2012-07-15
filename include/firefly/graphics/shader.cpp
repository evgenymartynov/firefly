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


// create a shader program from specified vert/fragment shaders

	GLuint ShaderMgr::CreateProgram(string vert, string frag, ...) 
	{
		// generate handles
		GLuint hProgram = 0;
		GLuint hVert = GL_DEBUG(glCreateShader(GL_VERTEX_SHADER));
		GLuint hFrag = GL_DEBUG(glCreateShader(GL_FRAGMENT_SHADER));

		// load shaders
		if (!LoadShader(hVert, vert) ||
		    !LoadShader(hFrag, frag)) 
		{
			GL_DEBUG(glDeleteShader(hVert));
			GL_DEBUG(glDeleteShader(hFrag));
			return 0;
		}

		// compile shaders
		GL_DEBUG(glCompileShader(hVert));
		GL_DEBUG(glCompileShader(hFrag));
		if (!CheckShaderCompile(hVert, vert) ||
		    !CheckShaderCompile(hFrag, frag)) 
		{
			GL_DEBUG(glDeleteShader(hVert));
			GL_DEBUG(glDeleteShader(hFrag));
			return 0;
		}

		// create the shader program
		hProgram = GL_DEBUG(glCreateProgram());
		GL_DEBUG(glAttachShader(hProgram, hVert));
		GL_DEBUG(glAttachShader(hProgram, hFrag));

		// bind attributes to their locations
		va_list attributes;
		va_start(attributes, frag);

		char * arg;
		int argCount = va_arg(attributes, int);
		for (int i = 0; i < argCount; ++i) 
		{
			int index = va_arg(attributes, int);
			arg = va_arg(attributes, char *);
			GL_DEBUG(glBindAttribLocation(hProgram, index, arg));
		}

		va_end(attributes);

		// finish linking the program, and clean up shaders
		GL_DEBUG(glLinkProgram(hProgram));
		GL_DEBUG(glDeleteShader(hVert));
		GL_DEBUG(glDeleteShader(hFrag));

		if (!CheckProgramLink(hProgram))
		{
			GL_DEBUG(glDeleteProgram(hProgram));
			return 0;
		}		

		g_Log.write(LOG_LOAD, "Shader program created > [%d] ('%s, '%s')",
								hProgram, vert.c_str(), frag.c_str());

		m_shaders.push_back(hProgram);
		return hProgram;
	}


// deletes all loaded programs

	void ShaderMgr::DeletePrograms()
	{
		if (m_shaders.empty())
            return;

        auto it = m_shaders.begin(), end = m_shaders.end();
        for (; it != end; ++it)
        {
            GL_DEBUG(glUseProgram(*it));
            GL_DEBUG(glDeleteProgram(*it));
        }

        m_shaders.clear();
	}


// internal helper function to retrieve shader source from file

    bool ShaderMgr::LoadShader(GLuint shader, string filename) 
	{
		vector<byte> buffer;
		string path(FF_SHADER_PATH + filename);
		ifstream file(path.c_str(), ios::in | ios::binary); 

		if (file.good())
        {
			// calculate file size in  bytes
			GLint fileSize = (GLint)file.tellg();
			file.seekg(0, ios::end);
            fileSize = (GLint)file.tellg() - fileSize;

			// resize byte buffer and read in data
			file.seekg(0, ios::beg);
			buffer.resize(fileSize);
			file.read(&buffer[0], fileSize);

			// pass the shader source to open gl
			const GLchar * src = &buffer[0];
			GL_DEBUG(glShaderSource(shader, 1, &src, &fileSize));
			return true;
        } 
		else 
		{
			// couldn't open the file or an error occurred
			g_Log.write(LOG_ERROR, "ShaderMgr::LoadShader > unable to"
                  " load file '%s'!", filename.c_str());
			return false;
		}
	}


// checks for any shader compilation errors

    bool ShaderMgr::CheckShaderCompile(GLint shader, const string & file)
    {
        GLint success = GL_FALSE;
        GL_DEBUG(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));

        if (success == GL_FALSE)
        {
			// get length of log
            //GL_DEBUG(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len));
			char infoLog[FF_LOG_MAX];
 
			// pass the info log to the app log file
            GL_DEBUG(glGetShaderInfoLog(shader, FF_LOG_MAX, NULL, infoLog));
            g_Log.write(LOG_ERROR, "ShaderMgr::CreateProgram > Failed"
                        " to compile '%s'!", file.c_str());
            g_Log.write(LOG_ERROR, "OGL INFO LOG\n%s", infoLog);
        }
	
		return success == GL_TRUE ? true : false;
    }


// checks for any program compilation errors

    bool ShaderMgr::CheckProgramLink(GLint program)
    {
        GLint success = GL_FALSE;
        GL_DEBUG(glGetProgramiv(program, GL_LINK_STATUS, &success));

        if (!success)
        {
			// get length of log
            //GL_DEBUG(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len));
			char infoLog[FF_LOG_MAX];

			// pass the info log to the app log file
            GL_DEBUG(glGetProgramInfoLog(program, FF_LOG_MAX, NULL, infoLog));
            g_Log.write(LOG_ERROR, "ShaderMgr::CreateProgram > Failed to "
                        "link program [%d]!", program);
            g_Log.write(LOG_ERROR, "OpenGL INFO LOG\n%s", infoLog);
        }

        return success == GL_TRUE ? true : false;
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

