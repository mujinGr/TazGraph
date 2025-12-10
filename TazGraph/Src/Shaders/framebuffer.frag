#version 330 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

void main()
{
	vec4 texColor = texture(screenTexture, texCoords);
	FragColor = vec4(1.0 - texColor.rgb, texColor.a);
}