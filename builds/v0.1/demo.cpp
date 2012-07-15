#include <firefly.hpp>
#include <firefly/core/random.hpp>
#include <firefly/graphics/shader.hpp>
#include <firefly/graphics/texture.hpp>
#include <firefly/graphics/primitive.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLTools.h>
GLBatch cube;

// global vars
GLuint  cubeTexture;
GLuint	cubeShader;
GLint   locTexture;
GLint   locLightPos;
GLint   locMV, locMVP, locNM;
GLint	locAmbient, locDiffuse, locSpecular;	

mat4 modelViewMatrix;
mat4 projectionMatrix;
ff::Cube crate;

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

////////////////////////////////////////////////////////////////////////

namespace ff {

// allocate resources

    bool App::Load()
    {
		// force size, regardless of firefly.ini
		SetSize(512, 512);

		// set up basic OpenGL parameters
		GL_DEBUG(glClearColor(0.09f, 0.09f, 0.09f, 1.f));
		GL_DEBUG(glEnable(GL_DEPTH_TEST));
		GL_DEBUG(glEnable(GL_CULL_FACE));
		
		// load textures
		cubeTexture = g_Texture.LoadTexture("crate.png");

		// load shaders
		cubeShader = g_Shader.CreateProgram("demo.vert", "demo.frag", 3,
											FF_ATTRIBUTE_VERTEX, "vVertex",
											FF_ATTRIBUTE_NORMAL, "vNormal",
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

		// create geometry
		crate.Create(1);

		return true;
	}


// cleanup resources
	
	void App::Exit()
    {
		g_Texture.DeleteTextures();
		g_Shader.DeletePrograms();
    }


// move, update objects etc. (check input if required)

    void App::Update(const delta_t dt, const delta_t elapsed)
    {
        mouse_info mi = GetMouse();
        SetWindowTitle("firefly-demo v%d.%d (FPS: %.2lf)"
                       " x-%d y-%d",
                       FF_MAJOR_VERSION, FF_MINOR_VERSION,
                       GetFPS(), mi.x, mi.y);
	}


// paint pretty pixels

    void App::Render(const delta_t dt, const delta_t elapsed)
    {	
		// clear buffer
		GL_DEBUG(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		
		// set OpenGL state
		GL_DEBUG(glBindTexture(GL_TEXTURE_2D, cubeTexture));
		GL_DEBUG(glUseProgram(cubeShader));
		
		// perform transforms
		modelViewMatrix = glm::translate(mat4(), vec3(0.0f, 0.0f, -8.f));
		modelViewMatrix = glm::rotate(modelViewMatrix, 30.0f * (float)elapsed, vec3(0.0f, 1.0f, 0.0f));
		modelViewMatrix = glm::rotate(modelViewMatrix, 15.0f * (float)elapsed, vec3(1.0f, 0.0f, 0.0f));
		mat3 normalMatrix(modelViewMatrix);

		// create point light
		vec3 vLightPos(-100, 100, 100);
		vec4 vAmbientColor(0.2f, 0.2f, 0.2f, 1.f);
		vec4 vDiffuseColor(1, 1, 1, 1);
		vec4 vSpecularColor(1, 1, 1, 1);

		// copy uniform information to shader
		GL_DEBUG(glUniformMatrix3fv(locNM , 1, GL_FALSE, glm::value_ptr(normalMatrix)));
		GL_DEBUG(glUniformMatrix4fv(locMV , 1, GL_FALSE, glm::value_ptr(modelViewMatrix)));
		GL_DEBUG(glUniformMatrix4fv(locMVP, 1, GL_FALSE, glm::value_ptr(projectionMatrix * modelViewMatrix)));
		GL_DEBUG(glUniform4fv(locAmbient, 1, &vAmbientColor[0]));
		GL_DEBUG(glUniform4fv(locDiffuse, 1, &vDiffuseColor[0]));
		GL_DEBUG(glUniform4fv(locSpecular, 1, &vSpecularColor[0]));
		GL_DEBUG(glUniform3fv(locLightPos, 1, &vLightPos[0]));
		GL_DEBUG(glUniform1i(locTexture, 0));

		// render geometry
		//cube.Draw();
		crate.Render();
	}


// resize GL viewport

    void App::Resize(int width, int height)
    {
		// set the whole application window to be the OpenGL canvas
        GL_DEBUG(glViewport(0, 0, width, height));

		// use a perspective projection for the viewport
		projectionMatrix = glm::perspective(35.f, (float) width / (float) height, 0.1f, 100.f);
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

			// save current frame buffer to file
			case GLFW_KEY_F12:
				g_App.Screenshot("test.bmp");
				break;
			}
        }
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
