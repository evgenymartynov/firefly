#version 130

out vec4 vFragColor;

uniform sampler2D texSampler;

smooth in vec2 vVaryingTexCoord;

void main(void)
{  
   vFragColor = texture(texSampler, vVaryingTexCoord);
}
