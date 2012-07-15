#include <firefly.hpp>
#include <firefly/core/random.hpp>
#include <firefly/graphics/shader.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <GLTools.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLBatch.h>
#include <GLGeometryTransform.h>
#include <GLShaderManager.h>

// global vars
GLFrame             viewFrame;
GLFrustum           viewFrustum;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

GLuint	shader, cubeShader;		
GLint   locInverseCamera;
GLint	locMVP;				
GLint	locMV;				
GLint	locNM;		
GLint   locTexture;
GLint   locCubeMap;
GLint	locCubeMVP;		
GLint   locSampler;

GLTriangleBatch sphereBatch;
GLBatch cubeBatch;
GLuint cubeTexture, tarnishTexture;

GLfloat angle = 0;
GLfloat dist = 5;

GLenum cube[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
				   GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
				   GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
				   GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
				   GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
				   GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

char * cubeFaces[6] = { "data/pos_x.tga", "data/neg_x.tga", 
					    "data/pos_y.tga", "data/neg_y.tga", 
					    "data/pos_z.tga", "data/neg_z.tga" }; 

// TODO
// dont register every callback... use defines to decide which callbacks you want to use

// Load a TGA as a 2D Texture. Completely initialize the state
bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	
	// Read the texture bits
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if(pBits == NULL) 
		return false;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
				 eFormat, GL_UNSIGNED_BYTE, pBits);
	
    free(pBits);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR || 
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
    
	return true;
}

////////////////////////////////////////////////////////////////////////

namespace ff {


// allocate resources

    bool App::Load()
    {
		glClearColor(0, 0, 0, 1.0f );
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_DEPTH_TEST);

		shaderManager.InitializeStockShaders();
		viewFrame.MoveForward(-5.0f);

		// make the sphere
		gltMakeSphere(sphereBatch, 1.0f, 128, 64);

		// make the skybox
		gltMakeCube(cubeBatch, 20);

		shader = g_Shader.CreateProgram(
			"reflect.vert", "reflect.frag", 3, 
			FF_ATTRIBUTE_VERTEX, "vVertex",
			FF_ATTRIBUTE_NORMAL, "vNormal",
			FF_ATTRIBUTE_TEXTURE0, "vTexture");

		cubeShader = g_Shader.CreateProgram(
			"skybox.vert", "skybox.frag", 1, 
			FF_ATTRIBUTE_VERTEX, "vVertex");

		// load shader uniforms
		locInverseCamera = glGetUniformLocation(shader, "mInverseCamera");
		locMVP = glGetUniformLocation(shader, "mvpMatrix");
		locMV  = glGetUniformLocation(shader, "mvMatrix");
		locNM  = glGetUniformLocation(shader, "normalMatrix");
		locTexture = glGetUniformLocation(shader, "cubeMap");
		locSampler = glGetUniformLocation(shader, "colorMap");
		//locCubeMap = glGetUniformLocation(cubeShader, "cubeMap");
		locCubeMVP = glGetUniformLocation(cubeShader, "mvpMatrix");

		// load tarnish texture
		glGenTextures(1, &tarnishTexture);
		glBindTexture(GL_TEXTURE_2D, tarnishTexture);
		LoadTGATexture("data/tarnish.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

		// load cube map
		glGenTextures(1, &cubeTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		GLbyte * pBytes;
		GLint iWidth, iHeight, iComponents;
		GLenum eFormat;

		for (int i = 0; i < 6; ++i) 
		{
			pBytes = gltReadTGABits(cubeFaces[i], &iWidth, &iHeight, &iComponents, &eFormat);
			glTexImage2D(cube[i], 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
			free(pBytes);
		}
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tarnishTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

		return true;
	}

// cleanup resources
	
	void App::Exit()
    {
		g_Shader.DeletePrograms();
    }


// move, update etc.

    void App::Update(const delta_t dt, const delta_t elapsed)
    {
        mouse_info mi = GetMouse();
        SetWindowTitle("firefly-demo v%d.%d (FPS: %.2lf)"
                       " x-%d y-%d",
                       FF_MAJOR_VERSION, FF_MINOR_VERSION,
                       m_fpsAvg, mi.x, mi.y);

		const float moveSpeed = 5;
		if (g_App.GetKey('W'))
			viewFrame.MoveForward(moveSpeed * dt);
		if (g_App.GetKey('S'))
			viewFrame.MoveForward(-moveSpeed * dt);
		if (g_App.GetKey('Q'))
			viewFrame.RotateLocalY(moveSpeed / 5 * dt);
		if (g_App.GetKey('E'))
			viewFrame.RotateLocalY(-moveSpeed / 5 * dt);
		if (g_App.GetKey('A'))
			viewFrame.MoveRight(moveSpeed * dt);
		if (g_App.GetKey('D'))
			viewFrame.MoveRight(-moveSpeed * dt);

		viewFrame.MoveRight(moveSpeed * dt);
		viewFrame.RotateLocalY(-moveSpeed / 5 * dt);
    }


// paint pretty pixels

    void App::Render(const delta_t dt, const delta_t elapsed)
    {	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		M3DMatrix44f mCamera;
		M3DMatrix44f mCameraRotOnly;
		M3DMatrix44f mInverseCamera;

		viewFrame.GetCameraMatrix(mCamera, false);
		viewFrame.GetCameraMatrix(mCameraRotOnly, true);
		m3dInvertMatrix44(mInverseCamera, mCameraRotOnly);

		modelViewMatrix.PushMatrix();
		modelViewMatrix.MultMatrix(mCamera);
			glEnable(GL_CULL_FACE);
			glUseProgram(shader);
			glUniformMatrix4fv(locInverseCamera, 1, GL_FALSE, mInverseCamera);
			glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
			glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
			glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
			glUniform1i(locTexture, 0);
			glUniform1i(locSampler, 1);
			sphereBatch.Draw();
			glDisable(GL_CULL_FACE);
		modelViewMatrix.PopMatrix();

		modelViewMatrix.PushMatrix();
		modelViewMatrix.MultMatrix(mCameraRotOnly);
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
			glUseProgram(cubeShader);
			//glUniform1i(locCubeMap, 0);
			glUniformMatrix4fv(locCubeMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
			cubeBatch.Draw();
		modelViewMatrix.PopMatrix();
	}


// resize GL viewport

    void App::Resize(int width, int height)
    {
        GL_DEBUG(glViewport(0, 0, width, height));
		viewFrustum.SetPerspective(35.0f, float(width)/float(height), 1.0f, 100.0f);
		projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
		transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	}


// handle input msgs

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
				}
			}
        }
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
