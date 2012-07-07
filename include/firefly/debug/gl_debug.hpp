#ifndef FIREFLY_GLDEBUG_HPP
#define FIREFLY_GLDEBUG_HPP

#include <string>
using std::string;

#if FF_DEBUG
#  define GL_DEBUG(GLfunc) (GLfunc); GLDebugFunction(__FILE__, __LINE__)
#  define GL_CHECK(GLfunc) (GLfunc), GLDebugFunction(__FILE__, __LINE__)
#else
#  define GL_DEBUG(GLfunc) (GLfunc)
#  define GL_CHECK(GLfunc) (GLfunc)
#endif

extern void GLDebugFunction(const string & file, unsigned int line);

#endif
