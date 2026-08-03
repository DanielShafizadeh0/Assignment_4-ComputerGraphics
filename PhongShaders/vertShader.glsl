#version 430

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 texCoords; // Fixed type for texture coordinates
layout (location = 2) in vec3 vertNormal;

out vec3 varyingNormal;
out vec3 varyingLightDir;
out vec3 varyingVertPos;
out vec2 tec;
out vec3 varyingColor;

struct PositionalLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
uniform bool heatMap;

void main(void) {
    varyingVertPos = (mv_matrix * vec4(vertPos, 1.0)).xyz;
    varyingLightDir = normalize(light.position - varyingVertPos);
    tec = texCoords;

    if (!heatMap) {
        varyingNormal = (norm_matrix * vec4(vertNormal, 0.0)).xyz;
    } else {
        varyingNormal = vertNormal;
    }

    varyingColor = vec3(0.5) + light.position * 0.5; // Corrected light position calculation
    gl_Position = proj_matrix * mv_matrix * vec4(vertPos, 1.0);
}
