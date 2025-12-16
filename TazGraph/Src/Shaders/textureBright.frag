#version 430

in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color; //rgb value

uniform sampler2D texture_sampler;


void main() {

    vec4 textureColor = texture(texture_sampler, fragmentUV);

    color = fragmentColor * textureColor;
}