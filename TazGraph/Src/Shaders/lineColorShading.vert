#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 bPos;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec4 bColor;

layout (location = 4) in float instanceWidth;



out vec4 vs_color;
out float vs_instanceWidth;

uniform mat4 u_ViewProjection;

void main() {
    if (gl_VertexID == 0) {
        // first endpoint
        gl_Position = u_ViewProjection * vec4(aPos, 1.0);
        vs_color = aColor;
    } else {
        // second endpoint
        gl_Position = u_ViewProjection * vec4(bPos, 1.0);
        vs_color = bColor;
    }
    vs_instanceWidth = instanceWidth;
}