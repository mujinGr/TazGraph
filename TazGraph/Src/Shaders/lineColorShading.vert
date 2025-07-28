#version 430

in vec3 vertexPosition; //vec3 is array of 3 floats
in vec4 vertexColor;

out vec4 vs_color;

uniform mat4 u_ViewProjection;

void main() {
    gl_Position = u_ViewProjection * vec4(vertexPosition.xyz, 1.0);

    vs_color = vertexColor;
}