#include <firefly.hpp>
#include <firefly/core/random.hpp>
#include <firefly/graphics/render.hpp>

#include <GLTools.h>
GLBatch cube, base, screen;

////////////////////////////////////////////////////////////////////////

namespace ff {

// global render variables
MatrixStack mv, proj;
Transform   transform;
Frame       cameraFrame;
Frame       cubeFrame;

// cube shader / textures
GLuint  cubeTexture, baseTexture;
GLuint	phongShader;
GLint   locTexture;
GLint   locLightPos;
GLint   locMV, locMVP, locNM;
GLint	locAmbient, locDiffuse, locSpecular;	

// blur shader / textures / buffers
#define BLUR_TEXTURE_COUNT 4
#define SCREEN_WIDTH       800
#define SCREEN_HEIGHT      600
#define BLUR_FRAME_DELAY   0
#define BLUR_MIN_DELTA     1
#define PIXEL_DATA_SIZE    (SCREEN_WIDTH * SCREEN_HEIGHT * 3 * sizeof(ff::ubyte))
GLuint  pbo;
GLuint  blurShader;
GLuint  blurTextures[BLUR_TEXTURE_COUNT];
GLint   locBlurMVP;
bool    blurEnabled, moveBlur;
float   blurTimer;
int     curBlurFrame;

// movement variables
#define JUMP_VEL          20
#define JUMP_GRAVITY      35
#define TURN_SPEED        60
#define MOVE_SPEED        10
float   jumpVel = 0;
bool    jumping = false;

// helper functions
void AdvanceBlurFrame() { curBlurFrame = ((curBlurFrame + 1) % BLUR_TEXTURE_COUNT); }
GLuint GetBlurFrame0(){ return (1 + ((curBlurFrame + 3) % BLUR_TEXTURE_COUNT)); }
GLuint GetBlurFrame1(){ return (1 + ((curBlurFrame + 2) % BLUR_TEXTURE_COUNT)); }
GLuint GetBlurFrame2(){ return (1 + ((curBlurFrame + 1) % BLUR_TEXTURE_COUNT)); }
GLuint GetBlurFrame3(){ return (1 + ((curBlurFrame) % BLUR_TEXTURE_COUNT)); }

/*
   [ firefly ] - OpenGL framework written by John Cramb (2012)
   =-._.-==-._.-==-._.-==-._.-==-._.-==-._.-==-._.-==-._.-==-._.-=

   This demo is a simple example of how to use this framework
   to power your 3D graphics application. Here are a few tips:
 
   1. What you need to provide
   ---------------------------
   This is easy, the functions provided below are all that you need
   to fill out in order for the framework to load/render/shutdown
   your application.

   2. Where do I put my files?
   ---------------------------
   All application files to be used with the framework should be 
   placed according to this directory tree:

   data/texture/ - any image files provided to LoadTexture()
   data/shader/  - any shader files provided to CreateProgram()
   data/screenshots/ - this is where your saved screenshots go
   
   3. How do I config the application?
   -----------------------------------
   The 'firefly.ini' file provides the framework with a level
   of configuration without having to recompile the application.
   Make any changes to the ini file, then restart the program
   and the framework will apply your desired settings.

   4. How do I query for user input?
   ---------------------------------
   There are two ways to do this, using the framework.

   EVENT BASED INPUT: By using the Input() callback function,
   as provided below in this source code you can handle every 
   state change for the mouse and keyboard as soon as it happens.

   QUERY BASED INPUT: This gives you the control to request the
   state of any key/mouse whenever you want. To do this you 
   use the following functions:

   g_App.GetKey(int code) - returns true if the key is held down
   
   g_App.GetMouse - it will return a mouse_info struct that
   will be filled out with the current state of the mouse. You
   can then access this struct for the information you require.

   5. Further information!
   -----------------------
   Feel free to email me at john@geekdownunder.net
   To best understand how the framework is used, please inspect
   the sample program provided below.

 */


// allocate resources

    bool App::Load()
    {
		// force size and vsync
		SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		SetVSync(true);

		// set up basic OpenGL parameters
		GL_DEBUG(glClearColor(0.3f, 0.3f, 0.3f, 1.0f));
		GL_DEBUG(glEnable(GL_DEPTH_TEST));
		
		// load textures
		cubeTexture = g_Texture.LoadTexture("crate.png");
		baseTexture = g_Texture.LoadTexture("concrete2.jpg", true);

		// load shaders
		phongShader = g_Shader.CreateProgram("texPhong.vert", "texPhong.frag", 3,
											FF_ATTRIBUTE_VERTEX, "vVertex",
											FF_ATTRIBUTE_NORMAL, "vNormal",
											FF_ATTRIBUTE_TEXTURE0, "vTexture0");

		blurShader = g_Shader.CreateProgram("blur.vert", "blur.frag", 2,
											FF_ATTRIBUTE_VERTEX, "vVertex",
											FF_ATTRIBUTE_TEXTURE0, "vTexture0");

		// get locations for shader uniforms
		locTexture = GL_DEBUG(glGetUniformLocation(phongShader, "texSampler"));
		locLightPos = GL_DEBUG(glGetUniformLocation(phongShader, "lightPos"));
		locMV = GL_DEBUG(glGetUniformLocation(phongShader, "mvMatrix"));
		locMVP = GL_DEBUG(glGetUniformLocation(phongShader, "mvpMatrix"));
		locNM = GL_DEBUG(glGetUniformLocation(phongShader, "normalMatrix"));
		locAmbient = GL_DEBUG(glGetUniformLocation(phongShader, "ambient"));
		locDiffuse = GL_DEBUG(glGetUniformLocation(phongShader, "diffuse"));
		locSpecular = GL_DEBUG(glGetUniformLocation(phongShader, "specular"));
		locBlurMVP = GL_DEBUG(glGetUniformLocation(blurShader, "mvpMatrix"));

		// create geometry
		gltMakeCube(cube, 1);
		screen.Begin(GL_TRIANGLE_STRIP, 4, 1);
			screen.MultiTexCoord2f(0, 0, 0);
			screen.Vertex3f(0, 0, 0);
			screen.MultiTexCoord2f(0, 1, 0);
			screen.Vertex3f((float)GetWidth(), 0, 0);
			screen.MultiTexCoord2f(0, 0, 1);
			screen.Vertex3f(0, (float)GetHeight(), 0);
			screen.MultiTexCoord2f(0, 1, 1);
			screen.Vertex3f((float)GetWidth(), (float)GetHeight(), 0);
		screen.End();
		const float baseSize = 40.0f;
		const float baseHeight = -3.f;
		base.Begin(GL_TRIANGLE_STRIP, 8, 1);
			base.MultiTexCoord2f(0, 0, 0);
			base.Vertex3f(-baseSize, baseHeight, baseSize);
			base.MultiTexCoord2f(0, baseSize, 0);
			base.Vertex3f(baseSize, baseHeight, baseSize);
			base.MultiTexCoord2f(0, 0, baseSize);
			base.Vertex3f(-baseSize, baseHeight, -baseSize);
			base.MultiTexCoord2f(0, baseSize, baseSize);
			base.Vertex3f(baseSize, baseHeight, -baseSize);
			base.MultiTexCoord2f(0, 0, 0);
			base.Vertex3f(-baseSize, baseHeight, -baseSize);
			base.MultiTexCoord2f(0, baseSize, 0);
			base.Vertex3f(baseSize, baseHeight, -baseSize);
			base.MultiTexCoord2f(0, 0, baseSize);
			base.Vertex3f(-baseSize, baseHeight + baseSize, -baseSize);
			base.MultiTexCoord2f(0, baseSize, baseSize);
			base.Vertex3f(baseSize, baseHeight + baseSize, -baseSize);
		base.End();

		// create pixel buffer
		glGenBuffers(1, &pbo);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
		glBufferData(GL_PIXEL_PACK_BUFFER, PIXEL_DATA_SIZE, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		// create blur textures
		glGenTextures(BLUR_TEXTURE_COUNT, blurTextures);
		for (int i = 0; i < BLUR_TEXTURE_COUNT; ++i)
		{
			glActiveTexture(GL_TEXTURE1 + i);
			glBindTexture(GL_TEXTURE_2D, blurTextures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		}

		// set initial variables
		curBlurFrame = 0;
		blurTimer = 0;
		blurEnabled = true;
		moveBlur = false;
		cameraFrame.MoveForward(-10);
		return true;
	}


// cleanup resources
	
	void App::Exit()
    {
		g_Texture.DeleteTextures();
		g_Shader.DeletePrograms();
		glDeleteBuffers(1, &pbo);
    }


// move, update objects etc. (check input if required)

    void App::Update(const delta_t dt, const delta_t elapsed)
    {
		// retrieve current mouse state
        mouse_info mi = GetMouse();
		string buffer;

		// write program information to window title
		if (blurEnabled) buffer = "[B]";
        SetWindowTitle("firefly-demo v%d.%d (FPS: %.2lf)"
                       " x-%d y-%d %s",
                       FF_MAJOR_VERSION, FF_MINOR_VERSION,
                       GetFPS(), mi.x, mi.y, buffer.c_str());

		// calculate movement rate and direction
		bool forceBlur = false;
		if (GetKey('A')) {
			cameraFrame.MoveRight(MOVE_SPEED*dt);
			forceBlur = true;
		}
		if (GetKey('D')) {
			cameraFrame.MoveRight(-MOVE_SPEED*dt);
			forceBlur = true;
		}
		if (GetKey('W')) {
			cameraFrame.MoveForward(MOVE_SPEED*dt);
			forceBlur = true;
		}
		if (GetKey('S')) {
			cameraFrame.MoveForward(-MOVE_SPEED*dt);
			forceBlur = true;
		}

		// calculate mouse delta
		float delta = (GetWidth() / 2.f) - mi.x;
		moveBlur = (abs(delta) > BLUR_MIN_DELTA || forceBlur) ? true : false;
		cameraFrame.RotateWorld( -delta * TURN_SPEED * dt / 4, 0, 1, 0);

		// keep the mouse centered
		glfwSetMousePos(GetWidth() / 2, GetHeight() / 2);

		// handle current jumping state
		if (jumping) {		
			jumpVel -= JUMP_GRAVITY * dt;
			cameraFrame.MoveUp(jumpVel * dt);

			// if we have landed
			if (cameraFrame.GetOriginY() < 0) {
				jumping = false;
				jumpVel = 0;

				// make sure we are positioned back at ground height
				float dist = cameraFrame.GetOriginY();
				cameraFrame.MoveUp(cameraFrame.GetOriginY() - dist);
			}
		}
	}


// paint pretty pixels

    void App::Render(const delta_t dt, const delta_t elapsed)
    {	
		// clear buffer and save matrix state
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// position camera
		mat4 camera;
		cameraFrame.GetCameraMatrix(camera);
		mv.PushMatrix(camera);

			// create point light
			vec4 vLightPos(sin(elapsed) * 10, 5, -8 + (cos(elapsed) * 15), 1);
			vec4 vLightEyePos;
			vec4 vAmbientColor(0.1f, 0.1f, 0.1f, 1);
			vec4 vDiffuseColor(1, 1, 1, 1);
			vec4 vSpecularColor(1, 1, 1, 1);

			// use smoothstep to animate the cube movement
			static float xPos;
			xPos = ((float)sin(elapsed * 3.1) + 1.0f) / 2.0f;
			xPos = (xPos) * (xPos) * (3.0f - 2.0f * (xPos));
			xPos = (-1.5f * xPos) + (1.5f * (1.0f - xPos));

			// copy uniform information to shader 
			GL_DEBUG(glUseProgram(phongShader));
			vLightEyePos = mv.Transform(vLightPos);
			GL_DEBUG(glUniformMatrix3fv(locNM , 1, GL_FALSE, transform.GetNormalMatrix() ));
			GL_DEBUG(glUniformMatrix4fv(locMV , 1, GL_FALSE, transform.GetModelView() ));
			GL_DEBUG(glUniformMatrix4fv(locMVP, 1, GL_FALSE, transform.GetMVP() ));
			GL_DEBUG(glUniform4fv(locAmbient, 1, &vAmbientColor[0]));
			GL_DEBUG(glUniform4fv(locDiffuse, 1, &vDiffuseColor[0]));
			GL_DEBUG(glUniform4fv(locSpecular, 1, &vSpecularColor[0]));
			GL_DEBUG(glUniform3fv(locLightPos, 1, &vLightEyePos[0]));
			GL_DEBUG(glUniform1i(locTexture, 0));

			// render the floor
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, baseTexture);
			base.Draw();

			// transform modelview to rotate cube
			mv.PushMatrix();
				mat4 cubePos = translate(mat4(), vec3(xPos, -0.5f, -15.f));
				cubePos = rotate(cubePos, 100.0f * (float)sin(elapsed), vec3(1.0f, 0.0f, 0.0f));
				cubePos = rotate(cubePos, 20.0f * (float)elapsed, vec3(0.0f, 1.0f, 0.0f));
				mv.MultMatrix(cubePos);

				// copy uniform information to shader for cube
				vLightEyePos = mv.Transform(vLightPos);
				GL_DEBUG(glUniform3fv(locLightPos, 1, &vLightEyePos[0]));
				GL_DEBUG(glUniformMatrix3fv(locNM , 1, GL_FALSE, transform.GetNormalMatrix() ));
				GL_DEBUG(glUniformMatrix4fv(locMV , 1, GL_FALSE, transform.GetModelView() ));
				GL_DEBUG(glUniformMatrix4fv(locMVP, 1, GL_FALSE, transform.GetMVP() ));
				glBindTexture(GL_TEXTURE_2D, cubeTexture);

				// render geometry
				cube.Draw();
			mv.PopMatrix();

			// draw cube at light position
			mv.PushMatrix();
				cubePos = translate(mat4(), vLightPos.xyz());
				mv.MultMatrix(cubePos);
				vLightEyePos = mv.Transform(vLightPos);
				GL_DEBUG(glUniform3fv(locLightPos, 1, &vLightEyePos[0]));
				GL_DEBUG(glUniformMatrix3fv(locNM , 1, GL_FALSE, transform.GetNormalMatrix() ));
				GL_DEBUG(glUniformMatrix4fv(locMV , 1, GL_FALSE, transform.GetModelView() ));
				GL_DEBUG(glUniformMatrix4fv(locMVP, 1, GL_FALSE, transform.GetMVP() ));
				glBindTexture(GL_TEXTURE_2D, cubeTexture);
				cube.Draw();
			mv.PopMatrix();

			// draw a stationary cube
			mv.PushMatrix();
				cubePos = translate(mat4(), vec3(-5, 0, 0));
				cubePos = rotate(cubePos, 45.0f, vec3(1, 0, 0));
				mv.MultMatrix(cubePos);
				vLightEyePos = mv.Transform(vLightPos);
				GL_DEBUG(glUniform3fv(locLightPos, 1, &vLightEyePos[0]));
				GL_DEBUG(glUniformMatrix3fv(locNM , 1, GL_FALSE, transform.GetNormalMatrix() ));
				GL_DEBUG(glUniformMatrix4fv(locMV , 1, GL_FALSE, transform.GetModelView() ));
				GL_DEBUG(glUniformMatrix4fv(locMVP, 1, GL_FALSE, transform.GetMVP() ));
				glBindTexture(GL_TEXTURE_2D, cubeTexture);
				cube.Draw();
			mv.PopMatrix();

		mv.PopMatrix();

		// update blur frame textures
		blurTimer += (float)dt;
		if (blurTimer > BLUR_FRAME_DELAY)
		{
			// save frame buffer to pbo
			glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
			glReadPixels(0, 0, g_App.GetWidth(), g_App.GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

			// save pixel buffer to texture and increment frame index
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
			glActiveTexture(GL_TEXTURE0 + GetBlurFrame0());
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_App.GetWidth(), g_App.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

			// reset iterator variables
			blurTimer = 0;
			AdvanceBlurFrame();
		}

		// use stored blur textures to generate frame
		if (blurEnabled && moveBlur) 
		{
			// render quad to the screen
			mat4 ortho = glm::ortho(0.0f, (float)g_App.GetWidth(), 0.0f, (float)g_App.GetHeight());
			glDisable(GL_DEPTH_TEST);
			glUseProgram(blurShader);
			glUniformMatrix4fv(locBlurMVP, 1, GL_FALSE, value_ptr(ortho));
			glUniform1i(glGetUniformLocation(blurShader, "blurFrame0"), GetBlurFrame0());
			glUniform1i(glGetUniformLocation(blurShader, "blurFrame1"), GetBlurFrame1());
			glUniform1i(glGetUniformLocation(blurShader, "blurFrame2"), GetBlurFrame2());
			glUniform1i(glGetUniformLocation(blurShader, "blurFrame3"), GetBlurFrame3());
			screen.Draw();
			glEnable(GL_DEPTH_TEST);
		}
	}


// resize GL viewport

    void App::Resize(int width, int height)
    {
		// set the whole application window to be the OpenGL canvas
        GL_DEBUG(glViewport(0, 0, width, height));

		// use a perspective projection for the viewport
		proj.PushMatrix(perspective(35.f, (float) width / (float) height, 0.1f, 1000.f));
		transform.SetMatrices(mv, proj);
	}


// handle input msgs

    void App::Input(const input & msg)
    {
        if (msg.source    == ff::input::ffKeyboard && 
			msg.key.state == ff::input::ffReleased)
        {
			switch(msg.key.code)
			{
			case GLFW_KEY_ESC:
				Quit();
				break;

			// toggle the blur effect using pixel buffer objects
			case 'B':
				blurEnabled = !blurEnabled;
				break;

			// get your jump on!
			case GLFW_KEY_SPACE:
					if (!jumping) {
						jumping = true;
						jumpVel = JUMP_VEL;
					}
					break;

			// save current frame buffer to file
			case GLFW_KEY_F12:
				g_App.Screenshot("test.bmp");
				break;
			}
        }
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
