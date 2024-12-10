#version 400

in vec3 vertexPosition; //vec2 is array of 2 floats
in vec4 vertexColor;
in vec2 vertexUV;

out vec4 fragmentColor;
out vec2 fragmentUV;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertexPosition.xyz, 1.0);

    fragmentColor = vertexColor;

    fragmentUV = vertexUV;
}