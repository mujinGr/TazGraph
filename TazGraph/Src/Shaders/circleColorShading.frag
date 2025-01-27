#version 330

in vec4 fragmentColor;
in vec2 fragmentUV;

uniform float u_radius;

out vec4 color;

// if you want to have from uniform modifyable size
//float roundRect(vec2 p, vec2 size, float radius) {
//    vec2 d = abs(p) - size + vec2(radius, radius);
//    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
//}

float roundRect(vec2 p, float radius) {
    vec2 d = abs(p) - vec2(1.0f) + vec2(radius, radius);
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}
void main() {
    vec2 center = vec2(0., 0.);
    vec2 normCoords = fragmentUV - center;

    float dist = roundRect(normCoords, u_radius);

    if(dist < 0.0) {
        color = fragmentColor;
    }
    else {
        discard;
    }
}