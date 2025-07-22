#version 430

in vec4 v_color;

out vec4 color; //rgb value

void main() {
    color = vec4(v_color.rgb, v_color.a);
}