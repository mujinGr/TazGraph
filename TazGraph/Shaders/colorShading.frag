#version 400

in vec4 fragmentColor;

out vec4 color; //rgb value

void main() {
    color = fragmentColor;
}