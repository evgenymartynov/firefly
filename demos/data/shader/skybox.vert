#version 130

in vec4 vVertex;

uniform mat4 mvpMatrix;
varying vec3 vVaryingTexCoord;
 
void main(void)
{
	// pass on texture coordinates
    vVaryingTexCoord = normalize(vVertex.xyz);

	// finally transform the geometry
	gl_Position = mvpMatrix * vVertex;
}