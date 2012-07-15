#include <firefly.hpp>
#include <firefly/core/random.hpp>
#include <firefly/graphics/render.hpp>

#include <GLTools.h>
#include <GLFrame.h>
GLBatch cube, base;
GLBatch screen;
GLBatch mirror;

////////////////////////////////////////////////////////////////////////

namespace ff {

// cube rendering global vars
GLuint  cubeTexture, baseTexture;
GLuint	cubeShader;
GLint   locTexture;
GLint   locLightPos;
GLint   locMV, locMVP, locNM;
GLint	locAmbient, locDiffuse, locSpecular;	

// global render variables
MatrixStack mv, proj;
Transform transform;
Frame viewFrame;

#define BLUR_TEXTURE_COUNT 4
#define SCREEN_WIDTH       800
#define SCREEN_HEIGHT      600
#define BLUR_FRAME_DELAY   0.05
#define PIXEL_DATA_SIZE    (SCREEN_WIDTH * SCREEN_HEIGHT * 3 * sizeof(ff::ubyte))

// global blur variables
GLuint  pbo;
GLuint  blurShader;
GLuint  blurTextures[BLUR_TEXTURE_COUNT];
GLint   locBlurMVP;
bool    blurEnabled;
float   blurTimer;
int     curBlurFrame;

// global mirror variables
GLuint  fbo, rboDepth;
GLuint  mirrorTexture;
GLuint  mirrorShader;
GLint   locMirrorMVP;
GLint   locMirrorTexture;

const GLfloat mirrorTexWidth = 400;
const GLfloat mirrorTexHeight = 300;

GLenum fboBuffs[] = { GL_COLOR_ATTACHMENT0 };
GLenum windowBuff[] = { GL_FRONT_LEFT };

// helper functions
void AdvanceBlurFrame() { curBlurFrame = ((curBlurFrame + 1) % BLUR_TEXTURE_COUNT); }
GLuint GetBlurFrame0(){ return (1 + ((curBlurFrame + 3) % BLUR_TEXTURE_COUNT)); }
GLuint GetBlurFrame1(){ return (1 + ((curBlurFrame + 2) % BLUR_TEXTURE_COUNT)); }
GLuint GetBlurFrame2(){ return (1 + ((curBlurFrame + 1) % BLUR_TEXTURE_COUNT)); }
GLuint GetBlurFrame3(){ return (1 + ((curBlurFrame) % BLUR_TEXTURE_COUNT)); }

void DrawWorld(ff::delta_t dt, ff::delta_t elapsed, bool blur)
{
	mv.PushMatrix();

	// create point light
	vec3 vLightPos(-10, 50, 20);
	vec4 vAmbientColor(0.2f, 0.2f, 0.2f, 1.f);
	vec4 vDiffuseColor(1, 1, 1, 1);
	vec4 vSpecularColor(1, 1, 1, 1);

	// use smoothstep to animate the cube movement
	static float xPos;
	xPos = ((float)sin(elapsed * 3.1) + 1.0f) / 2.0f;
	xPos = (xPos) * (xPos) * (3.0f - 2.0f * (xPos));
	xPos = (-1.5f * xPos) + (1.5f * (1.0f - xPos));

	// create model view matrix 8 units back from viewer
	mv.Translate(0.f, 0.f, -8.f);

	// copy uniform information to shader 
	GL_DEBUG(glUseProgram(cubeShader));
	GL_DEBUG(glUniformMatrix3fv(locNM , 1, GL_FALSE, transform.GetNormalMatrix() ));
	GL_DEBUG(glUniformMatrix4fv(locMV , 1, GL_FALSE, transform.GetModelView() ));
	GL_DEBUG(glUniformMatrix4fv(locMVP, 1, GL_FALSE, transform.GetMVP() ));
	GL_DEBUG(glUniform4fv(locAmbient, 1, &vAmbientColor[0]));
	GL_DEBUG(glUniform4fv(locDiffuse, 1, &vDiffuseColor[0]));
	GL_DEBUG(glUniform4fv(locSpecular, 1, &vSpecularColor[0]));
	GL_DEBUG(glUniform3fv(locLightPos, 1, &vLightPos[0]));
	GL_DEBUG(glUniform1i(locTexture, 0));

	// render base
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, baseTexture);
	base.Draw();
	mv.PopMatrix();
	mv.PushMatrix();

	// transform modelview to rotate cube
	mv.Translate(xPos, -0.5f, -8.f);
	mv.Rotate(100.0f * (float)sin(elapsed), 1.0f, 0.0f, 0.0f);
	mv.Rotate(20.0f * (float)elapsed, 0.0f, 1.0f, 0.0f);

	// copy uniform information to shader for cube
	GL_DEBUG(glUniformMatrix3fv(locNM , 1, GL_FALSE, transform.GetNormalMatrix() ));
	GL_DEBUG(glUniformMatrix4fv(locMV , 1, GL_FALSE, transform.GetModelView() ));
	GL_DEBUG(glUniformMatrix4fv(locMVP, 1, GL_FALSE, transform.GetMVP() ));
	glBindTexture(GL_TEXTURE_2D, cubeTexture);

	// render geometry
	cube.Draw();
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
	if (blur && blurEnabled) 
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
		//SetVSync(true);

		// set up basic OpenGL parameters
		GL_DEBUG(glClearColor(0.3f, 0.3f, 0.3f, 1.0f));
		GL_DEBUG(glEnable(GL_DEPTH_TEST));
		
		// load textures
		cubeTexture = g_Texture.LoadTexture("crate.png");
		baseTexture = g_Texture.LoadTexture("concrete2.jpg", true);

		// load shaders
		cubeShader = g_Shader.CreateProgram("cube.vert", "cube.frag", 3,
											FF_ATTRIBUTE_VERTEX, "vVertex",
											FF_ATTRIBUTE_NORMAL, "vNormal",
											FF_ATTRIBUTE_TEXTURE0, "vTexture0");

		blurShader = g_Shader.CreateProgram("blur.vert", "blur.frag", 2,
											FF_ATTRIBUTE_VERTEX, "vVertex",
											FF_ATTRIBUTE_TEXTURE0, "vTexture0");

		mirrorShader = g_Shader.CreateProgram("mirror.vert", "mirror.frag", 2,
											FF_ATTRIBUTE_VERTEX, "vVertex",
											FF_ATTRIBUTE_TEXTURE0, "vTexture0");

		// get locations for shader uniforms
		locTexture = GL_DEBUG(glGetUniformLocation(cubeShader, "texSampler"));
		locLightPos = GL_DEBUG(glGetUniformLocation(cubeShader, "lightPos"));
		locMV = GL_DEBUG(glGetUniformLocation(cubeShader, "mvMatrix"));
		locMVP = GL_DEBUG(glGetUniformLocation(cubeShader, "mvpMatrix"));
		locNM = GL_DEBUG(glGetUniformLocation(cubeShader, "normalMatrix"));
		locAmbient = GL_DEBUG(glGetUniformLocation(cubeShader, "ambient"));
		locDiffuse = GL_DEBUG(glGetUniformLocation(cubeShader, "diffuse"));
		locSpecular = GL_DEBUG(glGetUniformLocation(cubeShader, "specular"));
		locBlurMVP = GL_DEBUG(glGetUniformLocation(blurShader, "mvpMatrix"));
		locMirrorMVP = GL_DEBUG(glGetUniformLocation(mirrorShader, "mvpMatrix"));
		locMirrorTexture = GL_DEBUG(glGetUniformLocation(mirrorShader, "texSampler"));

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
		const float baseSize = 90.0f;
		const float baseHeight = -3.f;
		base.Begin(GL_TRIANGLE_STRIP, 12, 1);
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
			base.MultiTexCoord2f(0, 0, 0);
			base.Vertex3f(baseSize, baseHeight, baseSize);
			base.MultiTexCoord2f(0, baseSize, 0);
			base.Vertex3f(-baseSize, baseHeight, baseSize);
			base.MultiTexCoord2f(0, 0, baseSize);
			base.Vertex3f(baseSize, baseHeight + baseSize, baseSize);
			base.MultiTexCoord2f(0, baseSize, baseSize);
			base.Vertex3f(-baseSize, baseHeight + baseSize, baseSize);
		base.End();
		mirror.Begin(GL_TRIANGLE_STRIP, 4, 1);
			mirror.MultiTexCoord2f(0, 0, 0);
			mirror.Vertex3f(-2, -1.5, 0);
			mirror.MultiTexCoord2f(0, 1, 0);
			mirror.Vertex3f(2, -1.5, 0);
			mirror.MultiTexCoord2f(0, 0, 1);
			mirror.Vertex3f(-2, 1.5, 0);
			mirror.MultiTexCoord2f(0, 1, 1);
			mirror.Vertex3f(2, 1.5, 0);
		mirror.End();

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
		
		// create frame buffer for mirror
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		
		// create depth renderbuffer
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, mirrorTexWidth, mirrorTexHeight);
		
		// create reflection texture
		glGenTextures(1, &mirrorTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mirrorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mirrorTexWidth, mirrorTexHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// attach texture and depth render buffer to frame buffer
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture, 0);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		
		// check the frame buffer is complete
		GL_CHECK_FRAMEBUFFER(GL_DRAW_FRAMEBUFFER);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		return true;
	}


// cleanup resources
	
	void App::Exit()
    {
		// Make sure default FBO is bound
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		g_Texture.DeleteTextures();
		g_Shader.DeletePrograms();
		glDeleteBuffers(1, &pbo);
    }


// move, update objects etc. (check input if required)

    void App::Update(const delta_t dt, const delta_t elapsed)
    {
        mouse_info mi = GetMouse();
		string buffer;

		if (blurEnabled)
			buffer = "[BLUR ENABLED]";

        SetWindowTitle("firefly-demo v%d.%d (FPS: %.2lf)"
                       " x-%d y-%d %s",
                       FF_MAJOR_VERSION, FF_MINOR_VERSION,
                       GetFPS(), mi.x, mi.y, buffer.c_str());
	}


// paint pretty pixels

    void App::Render(const delta_t dt, const delta_t elapsed)
    {	
		// set position of mirror
		vec3 vMirrorPos(0, 1.75f, -14.f);
		vec3 vMirrorForward(0, 0, 0);
		
		mv.PushMatrix();
		mv.Translate(vMirrorPos);
		mv.Rotate(180, 0, 1, 0);
		mv.Rotate(-10, 1, 0, 0);
		mv.Scale(-1, 1, 1);
		
		// render scene from mirror's perspective
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glDrawBuffers(1, fboBuffs);
		glViewport(0, 0, mirrorTexWidth, mirrorTexHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawWorld(dt, elapsed, true);
		mv.PopMatrix();

		// reset frame buffers and scene
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDrawBuffers(1, windowBuff);
		glViewport(0, 0, g_App.GetWidth(), g_App.GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw the scene
		DrawWorld(dt, elapsed, false);

		// reposition to the mirror
		mv.PushMatrix();
		mv.Translate(vMirrorPos);
		mv.Rotate(15, 1, 0, 0);

		// render the mirror surface
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mirrorTexture);
		glUseProgram(mirrorShader);
		GL_DEBUG(glUniformMatrix4fv(locMirrorMVP, 1, GL_FALSE, transform.GetMVP()));
		GL_DEBUG(glUniform1i(locMirrorTexture, 0));
		mirror.Draw();
		mv.PopMatrix();
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

			// save current frame buffer to file
			case GLFW_KEY_F12:
				g_App.Screenshot("test.bmp");
				break;
			}
        }
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
