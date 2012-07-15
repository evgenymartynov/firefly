#version 130

in vec4 vVertex;
in vec2 vTexture0;

uniform mat4 mvpMatrix;

smooth out vec2 vVaryingTexCoord;
 
void main(void)
{
    // pass through texture details
    vVaryingTexCoord = vTexture0.st;

	// finally transform the geometry
	gl_Position = mvpMatrix * vVertex;
}