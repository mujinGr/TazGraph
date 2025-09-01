#version 430

//layout (location = 0) in float instanceWidth;

in vec3 vertexPosition; //vec3 is array of 3 floats
in vec4 vertexColor;


out vec4 vs_color;
//out float vs_instanceWidth;

uniform mat4 u_ViewProjection;

void main() {
    gl_Position = u_ViewProjection * vec4(vertexPosition.xyz, 1.0);

    vs_color = vertexColor;

    //vs_instanceWidth = instanceWidth;
}