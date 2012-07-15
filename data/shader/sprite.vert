#version 130

in vec4 vVertex;
in vec4 vColor;

uniform mat4 mvpMatrix;
uniform float timeStamp;
out vec4 vPointColor;

void main(void)
{
    vec4 vNewVertex = vVertex;
    vPointColor = vColor;

    // offset by running time, move sprites closer
    vNewVertex.z += timeStamp;

    // if out of range, adjust
    if (vNewVertex.z > -1.0)
        vNewVertex.z -= 999.0;

    // custom size adjustment
    gl_PointSize = 30.0 + (vNewVertex.z / sqrt(-vNewVertex.z));

    // if they are very small, fade them up
    if (gl_PointSize < 4.0) 
        vPointColor = smoothstep(0.0, 4.0, gl_PointSize) * vPointColor;

	// finally transform the new position
	gl_Position = mvpMatrix * vNewVertex;
}
