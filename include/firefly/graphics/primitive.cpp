#include <firefly/graphics/primitive.hpp>

////////////////////////////////////////////////////////////////////////

namespace ff {

// create and populate the cubes vbo/vao

	void Cube::Create(float size) 
	{
		static const GLfloat vertices[] =
		{ 
			// top
			size,  size,  size,
			size,  size, -size,
		   -size,  size, -size,
			size,  size,  size,
		   -size,  size, -size,
		   -size,  size,  size,

			// bottom
	       -size, -size, -size,
			size, -size, -size,
			size, -size,  size,
		   -size, -size,  size,
		   -size, -size, -size,
			size, -size,  size,

			// left
		   -size,  size,  size,
		   -size,  size, -size,
		   -size, -size, -size,
		   -size,  size,  size,
		   -size, -size, -size,
		   -size, -size,  size,

			// right
			size, -size, -size,
			size,  size, -size,
			size,  size,  size,
			size,  size,  size,
			size, -size,  size,
			size, -size, -size,

			// front
			size, -size,  size,
			size,  size,  size,
		   -size,  size,  size,
		   -size,  size,  size,
		   -size, -size,  size,
			size, -size,  size,

			// back
			size, -size, -size,
		   -size, -size, -size,
		   -size,  size, -size,
		   -size,  size, -size,
			size,  size, -size,
			size, -size, -size
		};

		static const GLfloat normals[] =
		{ 
			// top
			0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f,

			// bottom
			0.0f, -1.0f,  0.0f,
			0.0f, -1.0f,  0.0f,
			0.0f, -1.0f,  0.0f,
			0.0f, -1.0f,  0.0f,
			0.0f, -1.0f,  0.0f,
			0.0f, -1.0f,  0.0f,

			// left
		   -1.0f,  0.0f,  0.0f,
		   -1.0f,  0.0f,  0.0f,
		   -1.0f,  0.0f,  0.0f,
		   -1.0f,  0.0f,  0.0f,
		   -1.0f,  0.0f,  0.0f,
		   -1.0f,  0.0f,  0.0f,

			// right
			1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,

			// front
			0.0f,  0.0f,  1.0f,
			0.0f,  0.0f,  1.0f,
			0.0f,  0.0f,  1.0f,
			0.0f,  0.0f,  1.0f,
			0.0f,  0.0f,  1.0f,
			0.0f,  0.0f,  1.0f,

			// back
			0.0f,  0.0f, -1.0f,
			0.0f,  0.0f, -1.0f,
			0.0f,  0.0f, -1.0f,
			0.0f,  0.0f, -1.0f,
			0.0f,  0.0f, -1.0f,
			0.0f,  0.0f, -1.0f,
		};

		static const GLfloat texcoords[] =
		{ 
			// top
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,

			// bottom
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,

			// left
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,

			// right
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,

			// front
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,

			// back
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
		};

		// create the buffer object and array
		GL_DEBUG(glGenBuffers(1, &vbo));
		GL_DEBUG(glGenVertexArrays(1, &vao));
		GL_DEBUG(glBindVertexArray(vao));
		GL_DEBUG(glBindBuffer(GL_ARRAY_BUFFER, vbo));

		// store sizes
		vertSize = sizeof(vertices);
		normalSize = sizeof(normals);
		textureSize = sizeof(texcoords);

		// load data into the buffer
		GL_DEBUG(glBufferData(GL_ARRAY_BUFFER, vertSize + normalSize + textureSize, NULL, GL_STATIC_DRAW));
		GL_DEBUG(glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, normals));
		GL_DEBUG(glBufferSubData(GL_ARRAY_BUFFER, vertSize, normalSize, normals));
		GL_DEBUG(glBufferSubData(GL_ARRAY_BUFFER, vertSize + normalSize, textureSize, texcoords));

		// and unbind VAO and its parent buffer object
		GL_DEBUG(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GL_DEBUG(glBindVertexArray(0));
	}


// pass the draw call to OpenGL

	void Cube::Render()
	{	
		GL_DEBUG(glEnable(GL_VERTEX_ARRAY));

		// bind the buffer object / and its VAO with pointer information
		GL_DEBUG(glBindVertexArray(vao));	
		GL_DEBUG(glBindBuffer(GL_ARRAY_BUFFER, vbo));		

		// ensure our attrib pointers are enabled.
		GL_DEBUG(glEnableVertexAttribArray(FF_ATTRIBUTE_VERTEX));
		GL_DEBUG(glEnableVertexAttribArray(FF_ATTRIBUTE_NORMAL));
		GL_DEBUG(glEnableVertexAttribArray(FF_ATTRIBUTE_TEXTURE0));

		// define the Vertex/UV pointer informatin
		GL_DEBUG(glVertexAttribPointer(FF_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
		GL_DEBUG(glVertexAttribPointer(FF_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
		GL_DEBUG(glVertexAttribPointer(FF_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));

		// draw the primitive
		GL_DEBUG(glDrawArrays(GL_TRIANGLES, 0, 36));

		// disable to be clean
		GL_DEBUG(glDisableVertexAttribArray(FF_ATTRIBUTE_VERTEX));
		GL_DEBUG(glDisableVertexAttribArray(FF_ATTRIBUTE_NORMAL));
		GL_DEBUG(glDisableVertexAttribArray(FF_ATTRIBUTE_TEXTURE0));

		// and unbind VAO and its parent buffer object
		GL_DEBUG(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GL_DEBUG(glBindVertexArray(0));
	}

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////