#ifndef FIREFLY_MATRIX_HPP
#define FIREFLY_MATRIX_HPP

#include <firefly/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/matrix_access.hpp>
//#include <glm/gtc/matrix_inverse.hpp>

#if FF_DEBUG
	#define FF_DEBUG_MATRIX_CHECKS
	#include <firefly/debug/log.hpp>
#endif

// identity 4x4 matrix used for memcpy loading
static const float identity_4x4[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
	                                    0.0f, 1.0f, 0.0f, 0.0f,
			                            0.0f, 0.0f, 1.0f, 0.0f,
					                    0.0f, 0.0f, 0.0f, 1.0f };

#define FF_MATRIX_MAX_DEPTH 64
typedef glm::detail::tmat4x4<float> mat_t;

////////////////////////////////////////////////////////////////////////

namespace ff {

// set up scope of glm matrix functions

	using glm::frustum;
	using glm::lookAt;
	using glm::rotate;	
	using glm::transpose;
	using glm::determinant;
	using glm::transpose;
	using glm::translate;
	using glm::scale;
	using glm::ortho;
	using glm::perspective;
	using glm::perspectiveFov;

// helper class to handle matrix stacks

	class MatrixStack
	{
	public:
		MatrixStack();
		~MatrixStack();

		inline void LoadIdentity(void);
		inline void LoadMatrix(const mat4 & m);
		inline void LoadMatrix(const float * fv);
		inline void MultMatrix(const mat4 & m);

		inline void PushMatrix(void);
		inline void PushMatrix(const mat4 & m);
		inline void PopMatrix(void);

		inline void Translate(const float x, const float y, const float z);
		inline void Rotate(const float degrees, const float x, const float y, const float z);
		inline void Scale(const float x_scale, const float y_scale, const float z_scale);

		inline void Translate(const vec3 & fv);
		inline void Rotate(const float degrees, const vec3 & fv);
		inline void Scale(const vec3 & fv);

		inline vec4 Transform(const vec4 & fv) { return fv * m_stack.top(); }

		inline const mat4 & top(void);
		inline const float * ptr(void);
		inline void GetMatrix(mat4 & m);

		inline const mat3 mat3(void);

		operator mat_t () { return m_stack.top(); }
		mat_t operator * (MatrixStack & other) { return m_stack.top() * other.top(); }
		mat_t operator * (mat_t & other) { return m_stack.top() * other; }

	private:
		stack<glm::detail::tmat4x4<float>> m_stack;
	};


// matrix stack implementation, ctor/dtor

	MatrixStack::MatrixStack() {
		m_stack.push(mat4());
	}

	MatrixStack::~MatrixStack() {
	}


// loads the 4x4 identity matrix 

	inline void MatrixStack::LoadIdentity(void)
	{
		memcpy(value_ptr(m_stack.top()), identity_4x4, 16 * sizeof(float));
	}


// copies the provided matrix to the stack

	inline void MatrixStack::LoadMatrix(const mat4 & m)
	{
		memcpy(value_ptr(m_stack.top()), value_ptr(m), 16 * sizeof(float));
	}


// copies array of floats into current matrix

	inline void MatrixStack::LoadMatrix(const float * fv)
	{
		memcpy(value_ptr(m_stack.top()), fv, 16 * sizeof(float));
	}


// multiplies current matrix with provided one

	inline void MatrixStack::MultMatrix(const mat4 & m)
	{
		m_stack.top() *= m;
	}


// pushes on another copy of top matrix

	inline void MatrixStack::PushMatrix(void)
	{
		if (m_stack.size() < FF_MATRIX_MAX_DEPTH) {
			m_stack.push(m_stack.top());
		} 	
		#ifdef FF_DEBUG_MATRIX_CHECKS
		else {
			g_Log.write(LOG_ERROR, 
				"MatrixStack > stack overflow!");
		}
		#endif
	}


// pushes specified matrix onto the stack

	inline void MatrixStack::PushMatrix(const mat4 & m)
	{
		if (m_stack.size() < FF_MATRIX_MAX_DEPTH) {
			m_stack.push(m);
		} 	
		#ifdef FF_DEBUG_MATRIX_CHECKS
		else {
			g_Log.write(LOG_ERROR, 
				"MatrixStack > stack overflow!");
		}
		#endif
	}


// pops the top matrix off the stack

	inline void MatrixStack::PopMatrix(void)
	{
		if (m_stack.size() > 1) {
			m_stack.pop();
		} 
		#ifdef FF_DEBUG_MATRIX_CHECKS
		else {
			g_Log.write(LOG_ERROR, 
				"MatrixStack > stack underflow!");
		}
		#endif
	}


// applies a translation to the top matrix

	inline void MatrixStack::Translate(const float x, const float y, const float z)
	{
		mat_t & top = m_stack.top();
		top = translate(top, vec3(x, y, z));
	}


// applies a rotation to the top matrix

	inline void MatrixStack::Rotate(const float degrees, const float x, const float y, const float z)
	{
		mat_t & top = m_stack.top();
		top = rotate(top, degrees, vec3(x, y, z));
	}


// applies a scale to the top matrix

	inline void MatrixStack::Scale(const float x_scale, const float y_scale, const float z_scale)
	{
		mat_t & top = m_stack.top();
		top = scale(top, vec3(x_scale, y_scale, z_scale));
	}


// applies a translation using a vector

	inline void MatrixStack::Translate(const vec3 & fv)
	{
		mat_t & top = m_stack.top();
		top = translate(top, fv);
	}


// applies a rotation using vectors to top matrix

	inline void MatrixStack::Rotate(const float degrees, const vec3 & fv)
	{
		mat_t & top = m_stack.top();
		top = rotate(top, degrees, fv);
	}


// applies a scale to the top matrix using vectors

	inline void MatrixStack::Scale(const vec3 & fv) 
	{
		mat_t & top = m_stack.top();
		top = scale(top, fv);
	}


// returns a reference to top matrix

	inline const mat4 & MatrixStack::top(void)
	{
		return m_stack.top();
	}


// returns a float pointer to top matrix (useful for OpenGL)

	inline const float * MatrixStack::ptr(void)
	{
		return value_ptr(m_stack.top());
	}


// copies active matrix into provided one

	inline void MatrixStack::GetMatrix(mat4 & m) 
	{
		memcpy(value_ptr(m), value_ptr(m_stack.top()), 16 * sizeof(float));
	}


// returns a 3x3 matrix subset of the top matrix

	inline const mat3 MatrixStack::mat3(void)
	{
		return glm::mat3(m_stack.top());
	}

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
