#version 430

in vec3 vertexPosition; //vec2 is array of 2 floats
in vec4 vertexColor;
in vec2 vertexUV;

out vec3 fragmentPosition;
out vec4 fragmentColor;
out vec2 fragmentUV;

uniform mat4 u_ViewProjection;

void main() {
    gl_Position = u_ViewProjection * vec4(vertexPosition.xyz, 1.0);

    fragmentPosition = vertexPosition;

    fragmentColor = vertexColor;

    fragmentUV = vertexUV;
}