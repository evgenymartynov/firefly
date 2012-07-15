#version 130
 
out vec4 vFragColor;

uniform samplerCube cubeMap;
uniform sampler2D   colorMap;

smooth in vec3 vVaryingTexCoord;
smooth in vec2 vTexCoords;

void main(void)
{  
    vFragColor = texture(cubeMap, vVaryingTexCoord.stp);
    vFragColor *= texture(colorMap, vTexCoords);
}