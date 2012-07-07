#version 140

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
 
in  vec3 in_Position;
in  vec3 in_Color;
out vec3 ex_Color;
 
void main(void)
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
	ex_Color = in_Color;
}