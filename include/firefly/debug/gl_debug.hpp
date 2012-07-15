#ifndef FIREFLY_GLDEBUG_HPP
#define FIREFLY_GLDEBUG_HPP

#include <firefly/common.hpp>

#if FF_DEBUG
	#define GL_DEBUG(GLfunc) (GLfunc); GLDebugFunction(__FILE__, __LINE__)
	#define GL_CHECK_FRAMEBUFFER(GLtarget) GLDebugFramebuffer(GLtarget, __FILE__, __LINE__)
	#define SOIL_CHECK_FOR_GL_ERRORS
#else
	#define GL_DEBUG(GLfunc) (GLfunc)
#endif

extern void GLDebugFunction(const string & file, unsigned int line);
extern void GLDebugFramebuffer(GLuint target, const string & file, unsigned int line);

#endif
