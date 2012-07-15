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
GLTriangleBatch     sphereBatch;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

GLuint	shader;	
GLint	locAmbientColor;
GLint	locDiffuseColor;
GLint	locSpecularColor;
GLint	locLight;			
GLint	locMVP;				
GLint	locMV;				
GLint	locNM;		
GLint   locTexture;

GLuint texture;

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

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		shaderManager.InitializeStockShaders();
		viewFrame.MoveForward(4.0f);

		// Make the sphere
		gltMakeSphere(sphereBatch, 1.0f, 128, 64);

		shader = g_Shader.CreateProgram(
			"phong.vert", "phong.frag", 3, 
			FF_ATTRIBUTE_VERTEX, "vVertex",
			FF_ATTRIBUTE_NORMAL, "vNormal",
			FF_ATTRIBUTE_TEXTURE0, "vTexture0");

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		LoadTGATexture("data/CoolTexture.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

		locAmbientColor = glGetUniformLocation(shader, "ambientColor");
		locDiffuseColor = glGetUniformLocation(shader, "diffuseColor");
		locSpecularColor = glGetUniformLocation(shader, "specularColor");
		locLight = glGetUniformLocation(shader, "vLightPosition");
		locMVP = glGetUniformLocation(shader, "mvpMatrix");
		locMV  = glGetUniformLocation(shader, "mvMatrix");
		locNM  = glGetUniformLocation(shader, "normalMatrix");
		locTexture = glGetUniformLocation(shader, "colorMap");
		return true;
	}

// cleanup resources
	
	void App::Exit()
    {
		glDeleteTextures(1, &texture);
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
    }


// paint pretty pixels

    void App::Render(const delta_t dt, const delta_t elapsed)
    {	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		modelViewMatrix.PushMatrix(viewFrame);
		modelViewMatrix.Rotate(elapsed * 10.0f, 0.0f, 1.0f, 0.0f);

		GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };
		GLfloat vAmbientColor[] = { 0.2, 0.2, 0.2, 1 };
		GLfloat vDiffuseColor[] = { 1, 1, 1, 1 };
		GLfloat vSpecularColor[] = { 1, 1, 1, 1 };

		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(shader);
		glUniform4fv(locAmbientColor, 1, vAmbientColor);
		glUniform4fv(locDiffuseColor, 1, vDiffuseColor);
		glUniform4fv(locSpecularColor, 1, vSpecularColor);
		glUniform3fv(locLight, 1, vEyeLight);
		glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
		glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
		glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
		glUniform1i(locTexture, 0);
		sphereBatch.Draw();

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
