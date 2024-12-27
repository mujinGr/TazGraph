#version 330

in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color; //rgb value

void main() {
    vec2 center = vec2(0., 0.);
    vec2 normCoords = fragmentUV - center;
    
    float radius = 1.0; // Half the width/height

    float dist = length(normCoords);

    if (dist > radius) {
        discard;
    }else {
        float edgeWidth = 0.05; // Width of the edge where the color transitions to black
        float alpha = 1.0 - smoothstep(radius - edgeWidth, radius, dist); // Smooth transition to zero alpha at the edge
        color = vec4(fragmentColor.rgb * alpha, fragmentColor.a);
    }
}