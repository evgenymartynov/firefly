#ifndef FIREFLY_FRAME_HPP
#define FIREFLY_FRAME_HPP

#include <firefly/common.hpp>

////////////////////////////////////////////////////////////////////////

namespace ff {

// ortho normal frame class

	class Frame
	{
	public:
		Frame() : m_origin(0.0f, 0.0f, 0.0f), 
				  m_forward(0.0f, 0.0f, -1.0f),
				  m_up(0.0f, 1.0f, 0.0f) {}
		~Frame() {}

		// setter / getter functions
		inline void SetOrigin(float x, float y, float z) { m_origin = vec3(x, y, z); }
		inline void SetOrigin(const vec3 & fv) { m_origin = fv; }
		inline const vec3 & GetOrigin() { return m_origin; }
		inline float GetOriginX() { return m_origin.x; }
		inline float GetOriginY() { return m_origin.y; }
		inline float GetOriginZ() { return m_origin.z; }

		inline void SetForward(float x, float y, float z) { m_forward = vec3(x, y, z); }
		inline void SetForward(const vec3 & fv) { m_forward = fv; }
		inline const vec3 & GetForward() { return m_forward; }

		inline void SetUp(float x, float y, float z) { m_up = vec3(x, y, z); }
		inline void SetUp(const vec3 & fv) { m_up = fv; }
		inline const vec3 & GetUp() { return m_up; }

		// returns vector indicating axis of frame
		inline void GetXAxis(vec3 & fv);
		inline void GetYAxis(vec3 & fv);
		inline void GetZAxis(vec3 & fv);

		// helper translate functions
		inline void TranslateWorld(float x, float y, float z)
			{ m_origin += vec3(x, y, z); }
		inline void TranslateLocal(float x, float y, float z)
			{ MoveForward(z); MoveUp(y); MoveRight(x); }

		// helper functions for movement
		inline void MoveForward(float delta) { m_origin += m_forward * delta; }
		inline void MoveUp(float delta)      { m_origin += m_up * delta; }
		inline void MoveRight(float delta) 
		{ 
			m_origin += glm::cross(m_up, m_forward) * delta;
		}


// constructs the matrix for the frame

		void GetMatrix(mat4 & m, bool rotation = false)
		{
			// calculate right vector
			vec3 xAxis = glm::cross(m_up, m_forward);

			// set x column
			m[0] = vec4(xAxis, 0); 	

			// set y column
			m[1] = vec4(m_up, 0);

			// set z column
			m[2] = vec4(m_forward, 0);

			// translation component
			if (rotation) m[3] = vec4(m_origin, 1);
			else          m[3] = vec4(0, 0, 0, 1);
		}


// constructs the camera matrix for the frame

		void GetCameraMatrix(mat4 & m, bool rotation = false)
		{
			// calculate the rotation matrix
			// reverse z axis
			vec3 z_axis = -m_forward;

			// x axis is y cross z
			vec3 x_axis = glm::cross(m_up, z_axis);

			// matrix is transposed
			#define M(row,col) m[col][row]

				M(0, 0) = x_axis[0];
				M(0, 1) = x_axis[1];
				M(0, 2) = x_axis[2];
				M(0, 3) = 0.0;
				M(1, 0) = m_up[0];
				M(1, 1) = m_up[1];
				M(1, 2) = m_up[2];
				M(1, 3) = 0.0;
				M(2, 0) = z_axis[0];
				M(2, 1) = z_axis[1];
				M(2, 2) = z_axis[2];

				// no translation information
				if (rotation)
				{
					M(2, 3) = 0.0;
					M(3, 0) = 0.0;
					M(3, 1) = 0.0;
					M(3, 2) = 0.0;
					M(3, 3) = 1.0;
				}
				else 
				{
					// apply translation
					m *= glm::translate(mat4(), -m_origin);
				}

			#undef M
		}


// perform rotation around local x-axis

		void RotateLocalX(float degrees)
		{
			// construct rotation matrix
			vec3 x_axis = glm::cross(m_up, m_forward);
			mat3 rotationMatrix = mat3( glm::rotate(mat4(), degrees, x_axis) );

			// rotate y and z axis
			m_up = m_up * rotationMatrix;
			m_forward = m_forward * rotationMatrix;
		}


// perform rotation around local y-axis

		void RotateLocalY(float degrees)
		{
			// construct rotation matrix (rotate around up axis)
			mat3 rotationMatrix = mat3( glm::rotate(mat4(), degrees, m_up) );

			// rotate forward vector 
			m_forward = m_forward * rotationMatrix;
		}


// perform rotation around local z-axis

		void RotateLocalZ(float degrees)
		{
			// construct rotation matrix (rotate around forward axis
			mat3 rotationMatrix = mat3( glm::rotate(mat4(), degrees, m_forward) );

			// rotate up vector
			m_up = m_up * rotationMatrix;
		}


// resets the axes to ensure they are orthonormal

		void Normalize(void)
		{
			// recalculate vectors
			vec3 x_axis = glm::cross(m_up, m_forward);
			m_forward = glm::cross(x_axis, m_up);
			m_up = glm::normalize(m_up);
			m_forward = glm::normalize(m_forward);
		}


// perform a rotation in world coordinates

		void RotateWorld(float degrees, float x, float y, float z)
		{
			mat3 rotationMatrix = mat3( glm::rotate(mat4(), degrees, vec3(x, y, z)) );
			m_up = m_up * rotationMatrix;
			m_forward = m_forward * rotationMatrix;
		}


// rotate around local axis

		void RotateLocal(float degrees, float x, float y, float z)
		{
			vec3 local(x, y, z);
			vec3 world;
			LocalToWorld(local, world, true);
			RotateWorld(degrees, world.x, world.y, world.z);
		}


// 'un-transforms' a point back to its world coordinates

		void LocalToWorld(const vec3 & local, vec3 & world, bool rotation = false)
		{
			mat4 rotationMatrix;
			GetMatrix(rotationMatrix, true);
			world = world * mat3( rotationMatrix );

			// translate the point
			if (rotation) {
				world += m_origin;
			}
		}


// transform a point into local space coordinates

		void WorldToLocal(const vec3 & world, vec3 & local, bool rotation = false)
		{
			// translate the origin
			vec3 newWorld = world - m_origin;

			// create rotation matrix
			mat4 rotationMatrix;
			GetMatrix(rotationMatrix, true);

			// perform rotation based on the inverted matrix
			rotationMatrix = glm::inverse(rotationMatrix);
			local = local * mat3( rotationMatrix );
		}


// transforms a point using the frame matrix

		void TransformPoint(const vec3 & src, vec3 & dst)
		{
			mat4 m;
			GetMatrix(m);

			// rotate and translate
			dst.x = m[0][0] * src.x + m[1][0] * src.y + m[2][0] * src.z + m[3][0]; 
            dst.y = m[0][1] * src.x + m[1][1] * src.y + m[2][1] * src.z + m[3][1];
            dst.z = m[0][2] * src.x + m[1][2] * src.y + m[2][2] * src.z + m[3][2];
		}


// rotates a vector using the frame matrix

		void RotateVector(const vec3 & src, vec3 & dst)
		{
			mat4 m;
			GetMatrix(m, true);

			// rotate only
			dst.x = m[0][0] * src.x + m[1][0] * src.y + m[2][0] * src.z; 
            dst.y = m[0][1] * src.x + m[1][1] * src.y + m[2][1] * src.z;
            dst.z = m[0][2] * src.x + m[1][2] * src.y + m[2][2] * src.z;
		}


	private:
		vec3 m_origin;
		vec3 m_forward;
		vec3 m_up;
	};

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
