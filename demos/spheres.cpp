#include <firefly.hpp>
#include <firefly/core/random.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

// global vars
GLShaderManager shaderManager;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrustum viewFrustum;
GLGeometryTransform transformPipeline;

GLTriangleBatch torusBatch;
GLTriangleBatch sphereBatch;
GLBatch floorBatch;

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];
GLFrame cameraFrame;

#define JUMP_VEL  3
float jumpVel;
float gravity = 4;
bool jumping = false;
float turnSpeed = 45;

// TODO
// dont register every callback... use defines to decide which callbacks you want to use

////////////////////////////////////////////////////////////////////////

namespace ff {

    bool App::Load()
    {
		GL_DEBUG(glEnable(GL_DEPTH_TEST));
		GL_DEBUG(glClearColor(0, 0, 0, 1));

		SetSize(1024, 768);
		shaderManager.InitializeStockShaders();

		// create 3d objects
		gltMakeTorus(torusBatch, 0.4f, 0.15f, 60, 60);	
		gltMakeSphere(sphereBatch, 0.1f, 80, 40);
    	
		floorBatch.Begin(GL_LINES, 324);
		for(GLfloat x = -20.0; x <= 20.0f; x += 0.5) {
			floorBatch.Vertex3f(x, -0.55f, 20.0f);
			floorBatch.Vertex3f(x, -0.55f, -20.0f);
			floorBatch.Vertex3f(20.0f, -0.55f, x);
			floorBatch.Vertex3f(-20.0f, -0.55f, x);
        }
		floorBatch.End();  

		//rng_seed();
		for (int i = 0; i < NUM_SPHERES; ++i)
		{
			GLfloat x = (GLfloat)((rng(400) - 200) * .1f);
			GLfloat z = (GLfloat)((rng(400) - 200) * .1f);
			spheres[i].SetOrigin(x, 0, z);
		}

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

		float linear = 3;
		float deg = 90;
		float angular = glm::radians(deg);

		if (GetKey('A')) {
			cameraFrame.MoveRight(linear*dt);
		}
		if (GetKey('D')) {
			cameraFrame.MoveRight(-linear*dt);
		}
		if (GetKey('W')) {
			cameraFrame.MoveForward(linear*dt);
		}
		if (GetKey('S')) {
			cameraFrame.MoveForward(-linear*dt);
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
		static GLfloat vFloorColor[] = { 0, 1, 0, 1 };		
		static GLfloat vTorusColor[] = { 1, 0, 0, 1 };
		static GLfloat vSphereColor[] = { 0, 0, 1, 1 };
		float yRot = elapsed * 60.f;
		
		modelViewMatrix.PushMatrix();

		// create camera
		M3DMatrix44f camera;
		cameraFrame.GetCameraMatrix(camera);
		modelViewMatrix.PushMatrix(camera);

		// create light
		M3DVector4f vLightPos = { 0, 10, 5, 1 };
		M3DVector4f vLightEyePos;
		m3dTransformVector4(vLightEyePos, vLightPos, camera);

		// draw ground
		shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
									 transformPipeline.GetModelViewMatrix(),
									 transformPipeline.GetProjectionMatrix(),
									 vLightEyePos,
									 vFloorColor);
		floorBatch.Draw();

		// draw world spheres
		GL_DEBUG(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		for (int i = 0; i < NUM_SPHERES; ++i) {
			modelViewMatrix.PushMatrix();
			modelViewMatrix.MultMatrix(spheres[i]);
			shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
										 transformPipeline.GetModelViewMatrix(),
										 transformPipeline.GetProjectionMatrix(),
										 vLightEyePos,
										 vSphereColor);
			sphereBatch.Draw();
			modelViewMatrix.PopMatrix();
		}

		// draw torus
		modelViewMatrix.Translate(0, 0, -2.5);
		modelViewMatrix.PushMatrix();

		modelViewMatrix.Rotate(yRot, 0, 1, 0);
		shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
									 transformPipeline.GetModelViewMatrix(),
									 transformPipeline.GetProjectionMatrix(),
									 vLightEyePos,
									 vTorusColor);
		torusBatch.Draw();
		modelViewMatrix.PopMatrix();

		modelViewMatrix.Rotate(yRot * -2.f, 0, 1, 0);
		modelViewMatrix.Translate(0.8f, 0, 0);
		shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
								     transformPipeline.GetModelViewMatrix(),
									 transformPipeline.GetProjectionMatrix(),
									 vLightEyePos,
									 vSphereColor);
		sphereBatch.Draw();

		modelViewMatrix.PopMatrix();
		modelViewMatrix.PopMatrix();
	}


    void App::Resize(int width, int height)
    {
		viewFrustum.SetPerspective(35.0f, float(GetWidth()) / float(GetHeight()), .1f, 100.0f);
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
