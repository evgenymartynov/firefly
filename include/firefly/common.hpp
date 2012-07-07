#ifndef FIREFLY_COMMON_HPP
#define FIREFLY_COMMON_HPP

#define FF_MAJOR_VERSION 0
#define FF_MINOR_VERSION 1
#define FF_DEBUG 1

// C std lib
#include <cassert>
#include <cstdint>

// C++ STL libs
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using std::shared_ptr;
using std::stringstream;
using std::string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::size_t;

// gl math library
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

// firefly headers
#include <firefly/debug/log.hpp>

////////////////////////////////////////////////////////////////////////

#define FF_UNICODE 0

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

// useful constants
const uint32 MEGABYTE = 1024U * 1024U;

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
