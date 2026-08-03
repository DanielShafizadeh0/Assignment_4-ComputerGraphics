#version 430

in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec2 tec;
in vec3 varyingColor;

out vec4 fragColor;

uniform sampler2D s;

void main(void) {
    vec4 texColor = texture(s, tec);
    vec3 normal = normalize(varyingNormal);
    vec3 lightDir = normalize(varyingLightDir);

    // Simple diffuse lighting for testing
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = texColor.rgb * diff;

    fragColor = vec4(diffuse, 1.0);
}
