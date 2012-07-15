#version 130

in vec4 vVertex;
in vec3 vNormal;
in vec2 vTexture0;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPos;

smooth out vec3 vVaryingNormal;
smooth out vec3 vVaryingLightDir;
smooth out vec2 vVaryingTexCoord;
 
void main(void)
{
	// get surface normal in eye coordinates
	vVaryingNormal = normalMatrix * vNormal;

	// get vertex position in eye coordinates
	vec4 vPos4 = mvMatrix * vVertex;
	vec3 vPos3 = vPos4.xyz / vPos4.w;

	// get vector to light source
	vVaryingLightDir = normalize(lightPos - vPos3); 

    // pass through texture details
    vVaryingTexCoord = vTexture0.st;

	// finally transform the geometry
	gl_Position = mvpMatrix * vVertex;
}