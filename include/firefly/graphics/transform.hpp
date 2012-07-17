#ifndef FIREFLY_TRANSFORM_HPP
#define FIREFLY_TRANSFORM_HPP

#include <firefly/common.hpp>
#include <firefly/graphics/matrix.hpp>

////////////////////////////////////////////////////////////////////////

namespace ff {

	class Transform 
	{
	public:
		Transform() {}
		~Transform() {}

		inline void SetModelView(MatrixStack & mv)    { m_modelView = &mv; }
		inline void SetProjection(MatrixStack & proj) { m_projection = &proj; }
		inline void SetMatrices(MatrixStack & mv, MatrixStack & proj)
				              { SetModelView(mv); SetProjection(proj); }

		inline const float * GetModelView(void) { return m_modelView->ptr(); }
		inline const float * GetProj(void) { return m_projection->ptr(); }

		inline const float * GetMVP(void)
		{
			m_MVP = m_projection->top() * m_modelView->top();
			return value_ptr(m_MVP);
		}

		void GetNormalMatrix(mat3 & m, bool normalize = false) 
		{
			m_normalMatrix = glm::transpose( glm::inverse(m_modelView->mat3()) );
			if (normalize) {
				m_normalMatrix[0] = glm::normalize(m_normalMatrix[0]);
				m_normalMatrix[1] = glm::normalize(m_normalMatrix[1]);
				m_normalMatrix[2] = glm::normalize(m_normalMatrix[2]);
			}
			m = m_normalMatrix;
		}

		const float * GetNormalMatrix(bool normalize = false)
		{
			GetNormalMatrix(m_normalMatrix, normalize);
			return value_ptr(m_normalMatrix);
		}

	private:
		mat_t m_MVP;
		mat3 m_normalMatrix;
		MatrixStack * m_modelView;
		MatrixStack * m_projection;
	};

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
