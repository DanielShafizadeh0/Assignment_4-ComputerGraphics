#version 430

layout (location=0) in vec3 position;
layout (location=1) in vec2 tecCoord;
layout (location=2) in vec3  normal;
out vec3 vNormal;
out vec3 vVertPos;
out vec3 tc;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
layout (binding = 0) uniform sampler2D t;
out vec4 varyingColor;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vec4(position,1.0);
	varyingColor = vec4(position,1.0)*0.5 + vec4(0.5, 0.5, 0.5, 0.5);
        tc=tecCoord;
} 
