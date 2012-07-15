#ifndef FIREFLY_OPENGL_HPP
#define FIREFLY_OPENGL_HPP

#ifdef WIN32
    #include <Windows.h>
#endif

#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glfw.h>
#include <firefly/debug/gl_debug.hpp>

// firefly opengl type defines

typedef enum FF_SHADER_ATTRIBUTE {
	FF_ATTRIBUTE_VERTEX = 0,
	FF_ATTRIBUTE_COLOR,
	FF_ATTRIBUTE_NORMAL,
	FF_ATTRIBUTE_TEXTURE0,
	FF_ATTRIBUTE_TEXTURE1,
	FF_ATTRIBUTE_TEXTURE2,
	FF_ATTRIBUTE_TEXTURE3,
	FF_ATTRIBUTE_LAST,
};

#endif
