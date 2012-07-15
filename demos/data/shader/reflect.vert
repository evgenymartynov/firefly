#version 130

in vec4 vVertex;
in vec3 vNormal;
in vec2 vTexture;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform mat4 mInverseCamera;

smooth out vec3 vVaryingTexCoord;
smooth out vec2 vTexCoords;

void main(void)
{
	// normal in eye space
	vec3 vEyeNormal = normalMatrix * vNormal;

	// get vertex position in eye coordinates
	vec4 vVert4 = mvMatrix * vVertex;
	vec3 vEyeVertex = normalize(vVert4.xyz / vVert4.w);

	// get reflected vector
    vec4 vCoords = vec4(reflect(vEyeVertex, vEyeNormal), 1.0);

    // rotate by flipped camera
    vCoords = mInverseCamera * vCoords;
    vVaryingTexCoord.xyz = normalize(vCoords.xyz);

    // add texture coords
    vTexCoords = vTexture.st;

	// finally transform the geometry
	gl_Position = mvpMatrix * vVertex;
}