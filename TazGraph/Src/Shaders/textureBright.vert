#version 330

layout (location = 0) in vec3 vertexPosition; //vec2 is array of 2 floats
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec2 vertexUV;

out vec4 fragmentColor;
out vec2 fragmentUV;


uniform vec3 centerPosition;

uniform mat4 projection;


mat4 BuildTranslation(vec3 delta)
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(delta, 1.0));
}

void main() {

    mat4 translationOG  = BuildTranslation(centerPosition);


    gl_Position = projection * translationOG *  vec4(vertexPosition.xyz, 1.0);

    fragmentColor = vertexColor;

    fragmentUV = vertexUV;
}