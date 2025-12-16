#version 430

in vec4 fragmentColor;
in vec3 fragNormal;


out vec4 color; //rgb value

uniform vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
uniform vec3 lightColor = vec3(1.f);

void main() {

    vec3 normal = normalize(fragNormal);

    float diff = max(dot(normal, -lightDir), 0.5);

    vec3 diffuse = diff * lightColor;
    vec3 result = diffuse * fragmentColor.rgb;

    color = vec4(result, fragmentColor.a);
}