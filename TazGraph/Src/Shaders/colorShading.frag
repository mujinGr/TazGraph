#version 330

in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color; //rgb value

void main() {
    color = vec4(fragmentColor.rgb, fragmentColor.a);
}