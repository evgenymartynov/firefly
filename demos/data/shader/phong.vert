#version 130

in vec4 vVertex;
in vec3 vNormal;
in vec4 vTexture0;

uniform vec3 vLightPosition; // in eye coordinates
uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;

smooth out vec3 vVaryingNormal;
smooth out vec3 vVaryingLightDir;
smooth out vec2 vTexCoords;
 
void main(void)
{
	// get surface normal in eye coordinates
	vVaryingNormal = normalMatrix * vNormal;

	// get vertex position in eye coordinates
	vec4 vPosition4 = mvMatrix * vVertex;
	vec3 vPosition3 = vPosition4.xyz / vPosition4.w;

	// get vector to light source
	vVaryingLightDir = normalize(vLightPosition - vPosition3); 

    // pass through texture details
    vTexCoords = vTexture0.st;

	// finally transform the geometry
	gl_Position = mvpMatrix * vVertex;
}