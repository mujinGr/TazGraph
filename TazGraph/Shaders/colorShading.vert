#version 400

in vec2 vertexPosition; //vec2 is array of 2 floats

void main() {
    gl_Position = vec4(vertexPosition.xy,0.0, 1.0);
}