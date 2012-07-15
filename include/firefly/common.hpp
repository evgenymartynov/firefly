#ifndef FIREFLY_COMMON_HPP
#define FIREFLY_COMMON_HPP

#define FF_MAJOR_VERSION 0
#define FF_MINOR_VERSION 1
#define FF_DEBUG 1

#if defined(_DEBUG) && defined(WIN32)
	#include <stdlib.h>
	#include <crtdbg.h>
    #ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
    #endif
#endif

// C std lib
#include <cassert>
#include <cstdint>

// C++ STL libs
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <map>

using std::shared_ptr;
using std::unique_ptr;
using std::stringstream;
using std::string;
using std::vector;
using std::stack;
using std::map;
using std::pair;
using std::make_pair;
using std::size_t;

// gl math library
//#define GLM_MESSAGES
#define GLM_SWIZZLE
#define GLM_FORCE_INLINE
#define GLM_FORCE_CXX11
#define GLM_FORCE_SSE2
//#define GLM_PRECISION_MEDIUMP_INT
//#define GLM_PRECISION_HIGHP_FLOAT
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// firefly headers
#include <firefly/debug/log.hpp>

// useful constants
#define MEGABYTE 1048576
#define FF_PI (3.14159265358979323846)
#define FF_2PI (2.0 * PI)
#define FF_PI_DIV_180 (0.017453292519943296)
#define FF_INV_PI_DIV_180 (57.2957795130823229)

// compile time math macros
#define FF_RADIANS(x) ((x)*FF_PI_DIV_180)
#define FF_DEGREES(x) ((x)*FF_INV_PI_DIV_180)

////////////////////////////////////////////////////////////////////////

namespace ff {

	// set up scope of glm types
	using glm::vec2;
	using glm::vec3;
	using glm::vec4;
	using glm::dvec2;
	using glm::dvec3;
	using glm::dvec4;
	using glm::mat3;
	using glm::mat4;
	using glm::dmat3;
	using glm::dmat4;

	// set up scope of glm functions
	using glm::value_ptr;
	using glm::ceil;
	using glm::degrees;
	using glm::radians;
	using glm::clamp;
	using glm::normalize;
	using glm::reflect;
	using glm::refract;
	using glm::round;
	using glm::smoothstep;

	// single byte types
	typedef char             byte;
	typedef unsigned char    ubyte;

	// integer types
	typedef int8_t           int8;
	typedef int16_t          int16;
	typedef int32_t          int32;
	typedef int64_t          int64;
	typedef intmax_t         int_max;
	typedef intptr_t         int_ptr;

	// unsigned types
	typedef uint8_t          uint8;
	typedef uint16_t         uint16;
	typedef uint32_t         uint32;
	typedef uint64_t         uint64;
	typedef uintmax_t        uint_max;
	typedef uintptr_t        uint_ptr;

	// time types
	typedef double           delta_t;

	////////////////////////////////////////////////////////////////////////

	// useful global functions
	template<typename T>
	T bytes_to_megabytes(T bytes)
	{
		T mb = static_cast<double>(bytes) * 9.5367431640625e-07;
		return mb;
	}

	template<typename T>
	T megabytes_to_bytes(T mb)
	{
		return ( mb * MEGABYTE );
	}

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

// useful global macros
#if !defined(SAFE_DELETE)
#   define SAFE_DELETE(x) if (x) delete   x; x = NULL
#endif

#if !defined(SAFE_DELETE_ARRAY)
#   define SAFE_DELETE_ARRAY(x) if (x) delete[] x; x = NULL
#endif

// useful array macros
#define ELEMENT_SIZE(x)  (sizeof((x)[0]))
#define ELEMENT_COUNT(x) (sizeof(x) / ELEMENT_SIZE(x))
#define ELEMENT_LAST(x)  ((x)[ELEMENT_COUNT(x) - 1])
#define ARRAY_END(x)     ((x) + ELEMENT_COUNT(x))

#endif
