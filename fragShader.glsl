#version 430

in vec3 vNormal;
in vec3 vVertPos;
in vec2 tc;
out vec4 fragColor;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
//uniform mat4 norm_matrix;
layout (binding = 0) uniform sampler2D t;

void main(void)
{
        //vec3 r = -reflect(normalize(-vVertPos), normalize(vNormal));
        fragColor = texture(t,tc);
}
