#ifndef FIREFLY_PRIMITIVE_HPP
#define FIREFLY_PRIMITIVE_HPP

#include <firefly/opengl.hpp>
#include <firefly/common.hpp>

////////////////////////////////////////////////////////////////////////

namespace ff {

// abstract base class for making other primitives

	class Primitive {
	public:
		Primitive() {}
		~Primitive() {}

		virtual void Create(float size) = 0;
		virtual void Render() = 0;

	protected:
		GLuint vbo;
		GLuint vao;
	};


// the almighty 3D cube, used by programmers all over the world

	class Cube : public Primitive 
	{
	public:
		Cube() {}
		~Cube() {}

		virtual void Create(float size);
		virtual void Render();

	private:
		GLuint vertSize;
		GLuint normalSize;
		GLuint textureSize;
	};

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
