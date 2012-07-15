#include <firefly/opengl.hpp>
#include <firefly/debug/log.hpp>
#include <firefly/debug/gl_debug.hpp>


// standard OpenGL error checking function

void GLDebugFunction(const string & file, unsigned int line)
{
    GLenum errorCode = glGetError();

    if (errorCode != GL_NO_ERROR)
    {
        string error = "UNKNOWN";
        string desc  = "Invalid GL_ERROR.";

        switch (errorCode)
        {
        case GL_INVALID_ENUM :
        {
            error = "GL_INVALID_ENUM";
            desc = "illegal enumeration parameter provided as a"
                   " function argument.";
            break;
        }
        case GL_INVALID_VALUE :
        {
            error = "GL_INVALID_VALUE";
            desc = "illegal numeric parameter provided as a function"
                   " argument.";
            break;
        }
        case GL_INVALID_OPERATION :
        {
            error = "GL_INVALID_OPERATION";
            desc = "current state is not legal for the parameters given"
                   " to the command.";
            break;
        }
        case GL_OUT_OF_MEMORY :
        {
            error = "GL_OUT_OF_MEMORY";
            desc = "cannot allocated memory required for the command.";
            break;
        }
        case GL_STACK_OVERFLOW : // deprecated
        {
            error = "GL_STACK_OVERFLOW";
            desc = "pushing operation cannot be done because it would "
                   "overflow the limit of that stack's size.";
            break;
        }
        case GL_STACK_UNDERFLOW : // deprecated
        {
            error = "GL_STACK_UNDERFLOW";
            desc = "stack popping operation cannot be done because the"
                   " stack is already at its lowest point.";
            break;
        }
        case GL_INVALID_FRAMEBUFFER_OPERATION :
        {
            error = "GL_INVALID_FRAMEBUFFER_OPERATION";
            desc = "attempt to read from or write/render to a "
                   "framebuffer that is not complete (invalid FBO).";
            break;
        }
        } // end switch

        g_Log.write(LOG_ERROR, "(%s, %i) %s : %s",
                    file.substr(file.find_last_of("\\/") + 1).c_str(),
                    line,
                    error.c_str(),
                    desc.c_str());
    }
}


// handles status checking of frame buffer objects

void GLDebugFramebuffer(GLuint target, const string & file, unsigned int line)
{
	GLenum fboStatus = glCheckFramebufferStatus(target);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		string error = "UNKNOWN";
        string desc  = "Invalid frame buffer status error.";

		switch (fboStatus)
		{
		case GL_FRAMEBUFFER_UNDEFINED:
			error = "GL_FRAMEBUFFER_UNDEFINED";
            desc = "does a window exist?";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            desc = "check the status of each attachment!";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            desc = "must attach at least one buffer to the fbo.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            desc = "check that all attachments enabled via"
				   " glDrawBuffers exist in the fbo.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            desc = "check that all attachments enabled via"
				   " glReadBuffer exist in the fbo.";
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			error = "GL_FRAMEBUFFER_UNSUPPORTED";
            desc = "incompatible formats used in attached buffers.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            desc = "check that the number of samples for each "
				   "attachment is the same.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			error = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            desc = "check that the number of layers for each "
				   "attachment is the same.";
			break;
		}

		g_Log.write(LOG_ERROR, "(%s, %i) %s : %s",
            file.substr(file.find_last_of("\\/") + 1).c_str(),
            line,
            error.c_str(),
            desc.c_str());
	}
}