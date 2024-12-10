#version 400

in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color; //rgb value

void main() {

    float distance = length(fragmentUV);
    color = vec4(fragmentColor.rgb, fragmentColor.a * (pow(0.01, distance) - 0.01));
}