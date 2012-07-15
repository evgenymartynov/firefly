#version 130
 
out vec4 vFragColor;

in vec4 vPointColor;
in mat2 rotation_matrix;
uniform sampler2D sprite;

void main(void)
{  
    vFragColor = texture(sprite, gl_PointCoord) * vPointColor;
}