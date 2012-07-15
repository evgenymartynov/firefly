#version 130

out vec4 vFragColor;

uniform sampler2D blurFrame0;
uniform sampler2D blurFrame1;
uniform sampler2D blurFrame2;
uniform sampler2D blurFrame3;
smooth in vec2 vVaryingTexCoord;

void main(void)
{  
    vec4 blur0 = texture(blurFrame0, vVaryingTexCoord);
    vec4 blur1 = texture(blurFrame1, vVaryingTexCoord);
    vec4 blur2 = texture(blurFrame2, vVaryingTexCoord);
    vec4 blur3 = texture(blurFrame3, vVaryingTexCoord);

    vFragColor = (blur0 + blur1 + blur2 + blur3) / 4;
}
