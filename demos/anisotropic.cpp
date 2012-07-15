#include <firefly.hpp>
#include <firefly/core/random.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLBatch.h>
#include <GLGeometryTransform.h>

// global vars
GLShaderManager shaderManager;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrustum viewFrustum;
GLGeometryTransform transformPipeline;
GLFrame cameraFrame;

#define TEXTURE_COUNT 4
const char * texFiles[TEXTURE_COUNT] = { "data/floor.tga", "data/brick.tga", "data/ceiling.tga", "data/stone.tga" };
GLuint textures[TEXTURE_COUNT];

GLBatch ceilBatch, floorBatch, wallBatch;
int aLevel = 0;
float aMax = 0;

#define JUMP_VEL  3
float jumpVel;
float gravity = 4;
bool jumping = false;
float turnSpeed = 45;

#define SIZE_H 20
#define SIZE_V 1.8f

void ConfigTexture(int i, int aniLevel) {
	GL_DEBUG(glBindTexture(GL_TEXTURE_2D, textures[i]));
	GL_DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GL_DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GL_DEBUG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniLevel));
}

void LoadTextures(int aniLevel)
{
	GL_DEBUG(glGenTextures(TEXTURE_COUNT, textures));
	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		GL_DEBUG(glBindTexture(GL_TEXTURE_2D, textures[i]));
		GLint iWidth, iHeight, iComponents;
		GLenum eFormat;
		GLbyte * bits = gltReadTGABits(texFiles[i], &iWidth, &iHeight, &iComponents, &eFormat);

		ConfigTexture(i, 0);

		GL_DEBUG(glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, bits));
		GL_DEBUG(glGenerateMipmap(GL_TEXTURE_2D));
		free (bits);
	}
}

// TODO
// dont register every callback... use defines to decide which callbacks you want to use

////////////////////////////////////////////////////////////////////////

namespace ff {

    bool App::Load()
    {
		rng_seed();
		GL_DEBUG(glCullFace(GL_BACK));
		GL_DEBUG(glEnable(GL_CULL_FACE));
		GL_DEBUG(glEnable(GL_DEPTH_TEST));
		GL_DEBUG(glEnable(GL_TEXTURE_2D));
		GL_DEBUG(glClearColor(0, 0, 0, 1));

		SetSize(1024, 768);
		shaderManager.InitializeStockShaders();

		if (gltIsExtSupported("GL_EXT_texture_filter_anisotropic")) {
			GL_DEBUG(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aMax));
			g_Log.write(LOG_NOTIME, "App::Load > %.0fx Anistropic filtering supported.", aMax);
		} else {
			g_Log.write(LOG_NOTIME, "App::Load > Anistropic filtering NOT supported.");
		}

		LoadTextures(0);

		float size = 20.f;
		float by = -(SIZE_V / 3);
		float ty = (SIZE_V * 2) / 3;
		M3DVector3f v0 = { -SIZE_H, by, -SIZE_H };
		M3DVector3f v1 = { -SIZE_H, ty, -SIZE_H };
		M3DVector3f v2 = {  SIZE_H, ty, -SIZE_H };
		M3DVector3f v3 = {  SIZE_H, by, -SIZE_H };
		M3DVector3f v4 = { -SIZE_H, by, SIZE_H };
		M3DVector3f v5 = { -SIZE_H, ty, SIZE_H };
		M3DVector3f v6 = {  SIZE_H, ty, SIZE_H };
		M3DVector3f v7 = {  SIZE_H, by, SIZE_H };

		M3DVector3f normalUp = { 0, 1, 0 };
		M3DVector3f normalDown = { 0, -1, 0 };
		M3DVector3f normalForward = { 0, 0, -1 };

		floorBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);		
			floorBatch.MultiTexCoord2f(0, 0, 0);
			floorBatch.Vertex3fv(v4);
			floorBatch.MultiTexCoord2f(0, SIZE_H, 0);
			floorBatch.Vertex3fv(v7);
			floorBatch.MultiTexCoord2f(0, 0, SIZE_H);
			floorBatch.Vertex3fv(v0);
			floorBatch.MultiTexCoord2f(0, SIZE_H, SIZE_H);
			floorBatch.Vertex3fv(v3);
		floorBatch.End();

		ceilBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);	
			ceilBatch.MultiTexCoord2f(0, 0, 0);
			ceilBatch.Vertex3fv(v1);		
			ceilBatch.MultiTexCoord2f(0, SIZE_H, 0);
			ceilBatch.Vertex3fv(v2);
			ceilBatch.MultiTexCoord2f(0, 0, SIZE_H);
			ceilBatch.Vertex3fv(v5);
			ceilBatch.MultiTexCoord2f(0, SIZE_H, SIZE_H);
			ceilBatch.Vertex3fv(v6);
		ceilBatch.End();

		wallBatch.Begin(GL_TRIANGLES, 24, 1);
			wallBatch.MultiTexCoord2f(0, 0, 0);
			wallBatch.Vertex3fv(v0);
			wallBatch.MultiTexCoord2f(0, SIZE_H, 0);
			wallBatch.Vertex3fv(v3);
			wallBatch.MultiTexCoord2f(0, 0, SIZE_V);
			wallBatch.Vertex3fv(v1);
			wallBatch.MultiTexCoord2f(0, SIZE_H, 0);
			wallBatch.Vertex3fv(v3);
			wallBatch.MultiTexCoord2f(0, SIZE_H, SIZE_V);
			wallBatch.Vertex3fv(v2);
			wallBatch.MultiTexCoord2f(0, 0, SIZE_V);
			wallBatch.Vertex3fv(v1);
			
			wallBatch.MultiTexCoord2f(0, 0, 0);
			wallBatch.Vertex3fv(v3);
			wallBatch.MultiTexCoord2f(0, SIZE_H, 0);
			wallBatch.Vertex3fv(v7);
			wallBatch.MultiTexCoord2f(0, 0, SIZE_V);
			wallBatch.Vertex3fv(v2);
			wallBatch.MultiTexCoord2f(0, SIZE_H, 0);
			wallBatch.Vertex3fv(v7);
			wallBatch.MultiTexCoord2f(0, SIZE_H, SIZE_V);
			wallBatch.Vertex3fv(v6);
			wallBatch.MultiTexCoord2f(0, 0, SIZE_V);
			wallBatch.Vertex3fv(v2);
			
			wallBatch.MultiTexCoord2f(0, 0, 0);
			wallBatch.Vertex3fv(v4);
			wallBatch.MultiTexCoord2f(0, SIZE_H, 0);
			wallBatch.Vertex3fv(v0);
			wallBatch.MultiTexCoord2f(0, 0, SIZE_V);
			wallBatch.Vertex3fv(v5);
			wallBatch.MultiTexCoord2f(0, SIZE_H, 0);
			wallBatch.Vertex3fv(v0);
			wallBatch.MultiTexCoord2f(0, SIZE_H, SIZE_V);
			wallBatch.Vertex3fv(v1);
			wallBatch.MultiTexCoord2f(0, 0, SIZE_V);
			wallBatch.Vertex3fv(v5);
			
			wallBatch.MultiTexCoord2f(0, 0, 0);
			wallBatch.Vertex3fv(v7);
			wallBatch.MultiTexCoord2f(0, SIZE_H, 0);
			wallBatch.Vertex3fv(v4);
			wallBatch.MultiTexCoord2f(0, 0, SIZE_V);
			wallBatch.Vertex3fv(v6);
			wallBatch.MultiTexCoord2f(0, SIZE_H, 0);
			wallBatch.Vertex3fv(v4);
			wallBatch.MultiTexCoord2f(0, SIZE_H, SIZE_V);
			wallBatch.Vertex3fv(v5);
			wallBatch.MultiTexCoord2f(0, 0, SIZE_V);
			wallBatch.Vertex3fv(v6);
		wallBatch.End();

        return true;
    }


    bool App::Exit()
    {
		GL_DEBUG(glDeleteTextures(TEXTURE_COUNT, textures));
        return true;
    }


    void App::Update(const delta_t dt, const delta_t elapsed)
    {
        mouse_info mi = GetMouse();
        SetWindowTitle("firefly-demo v%d.%d (FPS: %.2lf)"
                       " x-%d y-%d",
                       FF_MAJOR_VERSION, FF_MINOR_VERSION,
                       m_fpsAvg, mi.x, mi.y);

		float linear = 3;
		float deg = 90;
		float angular = glm::radians(deg);

		float lpos = linear * dt;
		float lneg = -lpos;
		
		if (GetKey('A')) {
			cameraFrame.MoveRight(lpos);
		}
		if (GetKey('D')) {
			cameraFrame.MoveRight(lneg);
		}
		if (GetKey('W')) {
			cameraFrame.MoveForward(lpos);
		}
		if (GetKey('S')) {
			cameraFrame.MoveForward(lneg);
		}

		float delta = (GetWidth() / 2.f) - mi.x;
		float turn = delta * turnSpeed;
		cameraFrame.RotateWorld( turn * dt / 4, 0, 1, 0);
		glfwSetMousePos(GetWidth() / 2, GetHeight() / 2);

		if (jumping) {		
			jumpVel -= gravity * dt;
			cameraFrame.MoveUp(jumpVel * dt);

			if (cameraFrame.GetOriginY() < 0) {
				jumping = false;
				jumpVel = 0;
				float dist = cameraFrame.GetOriginY();
				if (dist > 0) cameraFrame.MoveUp(cameraFrame.GetOriginY() - dist);
				else cameraFrame.MoveUp(cameraFrame.GetOriginY() - dist);
			}
		}
    }


    void App::Render(const delta_t dt, const delta_t elapsed)
    {	
		float vWhite[] = { 1, 1, 1, 1 };
		modelViewMatrix.PushMatrix();

		// create camera and push on its matrix
		M3DMatrix44f camera;
		cameraFrame.GetCameraMatrix(camera);
		modelViewMatrix.PushMatrix(camera);
		
		// createlight source
		M3DVector4f vLightPos = { 0, 0, 0, 1 };
		M3DVector4f vLightEyePos;
		m3dTransformVector4(vLightEyePos, vLightPos, camera);

		GL_DEBUG(glBindTexture(GL_TEXTURE_2D, textures[0]));
		shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
									 transformPipeline.GetModelViewMatrix(),
									 transformPipeline.GetProjectionMatrix(),
									 vLightEyePos,
									 vWhite);
		floorBatch.Draw();

		GL_DEBUG(glBindTexture(GL_TEXTURE_2D, textures[2]));
		shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
									 transformPipeline.GetModelViewMatrix(),
									 transformPipeline.GetProjectionMatrix(),
									 vLightEyePos,
									 vWhite);
		ceilBatch.Draw();

		GL_DEBUG(glBindTexture(GL_TEXTURE_2D, textures[1]));
		shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
									 transformPipeline.GetModelViewMatrix(),
									 transformPipeline.GetProjectionMatrix(),
									 vLightEyePos,
									 vWhite);
		wallBatch.Draw();

		modelViewMatrix.PopMatrix();
		modelViewMatrix.PopMatrix();
	}


    void App::Resize(int width, int height)
    {
		viewFrustum.SetPerspective(35.f, (float)width / (float)height, 0.1f, 100.f);
		projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
		transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
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
				case 'T':
					aLevel += 4;
					if (aLevel > aMax) 
						aLevel = 0;
					for (int i = 0; i < TEXTURE_COUNT; ++i) {
						ConfigTexture(i, aLevel);
					}
					break;
				case GLFW_KEY_SPACE:
					if (!jumping) {
						jumping = true;
						jumpVel = JUMP_VEL;
					}
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
