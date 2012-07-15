#include <firefly.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <GLTools.h>
#include <GLShaderManager.h>
#include <cmath>

// global vars
GLBatch squareBatch;
GLShaderManager shaderManager;
GLBatch mesh;

// TODO
// dont register every callback... use defines to decide which callbacks you want to use

#define DIST_FROM_CENTER 0.5f

#define H_SCALE 0.275f
#define V_SCALE 0.6f
#define DIAMOND_TOP	     0,  V_SCALE, 0
#define DIAMOND_BOTTOM   0, -V_SCALE, 0
#define DIAMOND_LEFT    -H_SCALE, 0, 0
#define DIAMOND_RIGHT    H_SCALE, 0, 0
#define DIAMOND_FRONT    0, 0,  H_SCALE
#define DIAMOND_BACK     0, 0, -H_SCALE

#define C_BLACK   0, 0, 0, 1
#define C_WHITE   1, 1, 1, 1
#define C_RED     1, 0, 0, 1
#define C_GREEN   0, 1, 0, 1
#define C_BLUE    0, 0, 1, 1
#define C_YELLOW  1, 1, 0, 1
#define C_MAGENTA 1, 0, 1, 1
#define C_CYAN    0, 1, 1, 1

#define COLOR_TOP	   C_RED
#define COLOR_BOTTOM   C_BLUE
#define COLOR_LEFT     C_GREEN
#define COLOR_RIGHT    C_YELLOW
#define COLOR_FRONT    C_CYAN
#define COLOR_BACK     C_MAGENTA

GLfloat vVerts[] = { DIAMOND_TOP, DIAMOND_FRONT, DIAMOND_RIGHT,
					 DIAMOND_TOP, DIAMOND_RIGHT, DIAMOND_BACK,
					 DIAMOND_TOP, DIAMOND_BACK, DIAMOND_LEFT,
					 DIAMOND_TOP, DIAMOND_LEFT, DIAMOND_FRONT,
					 DIAMOND_BOTTOM, DIAMOND_FRONT, DIAMOND_LEFT,
					 DIAMOND_BOTTOM, DIAMOND_RIGHT, DIAMOND_FRONT,
					 DIAMOND_BOTTOM, DIAMOND_BACK, DIAMOND_RIGHT,
					 DIAMOND_BOTTOM, DIAMOND_LEFT, DIAMOND_BACK,
					};
GLfloat vColor[] = { COLOR_TOP, COLOR_FRONT, COLOR_RIGHT,
					 COLOR_TOP, COLOR_RIGHT, COLOR_BACK,
					 COLOR_TOP, COLOR_BACK, COLOR_LEFT,
					 COLOR_TOP, COLOR_LEFT, COLOR_FRONT,
				     COLOR_BOTTOM, COLOR_FRONT, COLOR_LEFT,
					 COLOR_BOTTOM, COLOR_RIGHT, COLOR_FRONT,
					 COLOR_BOTTOM, COLOR_BACK, COLOR_RIGHT,
					 COLOR_BOTTOM, COLOR_LEFT, COLOR_BACK,
					};

GLfloat rotation[3] = { 5, 0, 0 };
GLfloat degreesPerSec = 90;
GLfloat circleSpeed = 1.8;
GLfloat spinSpeed = 120;
GLfloat arcPos = 0;
GLint wireframe = 2;
mat4 matP;

////////////////////////////////////////////////////////////////////////

namespace ff {

    bool App::Load()
    {
		GL_DEBUG(glEnable(GL_DEPTH_TEST));
		GL_DEBUG(glClearColor(.2f, .2f, .2f, 1));
		GL_DEBUG(glCullFace(GL_BACK));
		GL_DEBUG(glEnable(GL_CULL_FACE));
		GL_DEBUG(glEnable(GL_MULTISAMPLE));

		SetSize(512, 512);
		shaderManager.InitializeStockShaders();

		mesh.Begin(GL_TRIANGLES, 24);
		mesh.CopyVertexData3f(vVerts);
		mesh.CopyColorData4f(vColor);
		mesh.End();

        return true;
    }


    bool App::Exit()
    {

        return true;
    }


    void App::Update(const delta_t dt, const delta_t elapsed)
    {
        mouse_info mi = GetMouse();
        SetWindowTitle("firefly-demo v%d.%d (FPS: %.2lf)"
                       " x-%d y-%d",
                       FF_MAJOR_VERSION, FF_MINOR_VERSION,
                       m_fpsAvg, mi.x, mi.y);

		if (GetKey(GLFW_KEY_LEFT)) {
			rotation[1] -= dt * degreesPerSec;
		}
		if (GetKey(GLFW_KEY_RIGHT)) {
			rotation[1] += dt * degreesPerSec;
		}
		if (GetKey(GLFW_KEY_UP)) {
			rotation[0] -= dt * degreesPerSec;
		}
		if (GetKey(GLFW_KEY_DOWN)) {
			rotation[0] += dt * degreesPerSec;
		}

		arcPos += circleSpeed * dt;
		rotation[1] += spinSpeed * dt;
    }

	void DrawScene() 
	{
		mesh.Draw();
	}


    void App::Render(const delta_t dt, const delta_t elapsed)
    {
		GLfloat vRed[] = { 1, 0, 0, 1 };
		GLfloat vWhite[] = { 1, 1, 1, 1 };
		GLfloat vBlack[] = { 0, 0, 0, 1 };

		GLfloat x = sinf(arcPos) * DIST_FROM_CENTER;
		GLfloat z = cosf(arcPos) * DIST_FROM_CENTER;

		mat4 T  = glm::translate(mat4(1.0f), vec3(x, 0, -1.5 + z)); 
		mat4 Rx = glm::rotate(T,  rotation[0], vec3(1.0f, 0.0f, 0.0f));
		mat4 V  = glm::rotate(Rx, rotation[1], vec3(0.0f, 1.0f, 0.0f));
		mat4 mvp = P * V;

		switch (wireframe) 
		{
		case 0:
			shaderManager.UseStockShader(GLT_SHADER_SHADED, &mvp[0]);
			GL_DEBUG(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
			GL_DEBUG(glEnable(GL_CULL_FACE));
			break;
		case 1:
			shaderManager.UseStockShader(GLT_SHADER_FLAT, &mvp[0], vWhite);
			GL_DEBUG(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
			GL_DEBUG(glLineWidth(1));
			GL_DEBUG(glDisable(GL_CULL_FACE));
			break;
		case 2:
			GL_DEBUG(glPolygonOffset(-1, -1));
			GL_DEBUG(glEnable(GL_POLYGON_OFFSET_LINE));
			shaderManager.UseStockShader(GLT_SHADER_FLAT, &mvp[0], vBlack);
			GL_DEBUG(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
			GL_DEBUG(glLineWidth(3));
			DrawScene();
			GL_DEBUG(glEnable(GL_CULL_FACE));
			GL_DEBUG(glDisable(GL_POLYGON_OFFSET_LINE));
			shaderManager.UseStockShader(GLT_SHADER_SHADED, &mvp[0]);
			GL_DEBUG(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
			break;
		}

		DrawScene();
	}


    void App::Resize(int width, int height)
    {
		P = glm::perspective(70.0f, (float) GetWidth() / (float) GetHeight(), 0.1f, 100.f);
    }


    void App::Input(const input & msg)
    {
        if (msg.source == input::ffKeyboard)
        {
			if (msg.key.state == ff::input::ffReleased)
			{
				switch(msg.key.code)
				{
				case GLFW_KEY_ESC:
					Quit();
					break;
				case GLFW_KEY_SPACE:
					++wireframe;
					if (wireframe > 2) wireframe = 0;
					break;
				}
			}
			else 
			{
				switch(msg.key.code)
				{
				case GLFW_KEY_LEFT:
					break;
				case GLFW_KEY_RIGHT:
					break;
				case GLFW_KEY_UP:
					break;
				case GLFW_KEY_DOWN:
					break;
				}
			}
        }
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
