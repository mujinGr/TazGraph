#version 430

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 vs_color[];      // From vertex shader
out vec4 v_color;        // To fragment shader

uniform float lineWidth = 10.0;    // Base line width in world units
uniform vec2 viewportSize = vec2(800.0, 600.0);
uniform float useWorldWidth = 1.0; // 1.0 = world space width, 0.0 = screen space width

void main() {
    vec4 start = gl_in[0].gl_Position;
    vec4 end = gl_in[1].gl_Position;
    
    // Clip space to NDC
    vec2 p0 = start.xy / start.w;
    vec2 p1 = end.xy / end.w;
    
    // Check for degenerate line
    if (distance(p0, p1) < 0.0001) {
        return;
    }
    
    // Calculate line width in clip space
    float width0, width1;
    
    if (useWorldWidth > 0.5) {
        // 3D perspective: width decreases with distance (larger w = farther)
        // Convert world-space width to clip-space width
        width0 = lineWidth / start.w * viewportSize.y * 0.5;  // Perspective scaling
        width1 = lineWidth / end.w * viewportSize.y * 0.5;
    } else {
        // 2D screen-space: constant pixel width
        width0 = lineWidth;
        width1 = lineWidth;
    }
    
    // NDC to screen space
    vec2 s0 = 0.5 * (p0 + 1.0) * viewportSize;
    vec2 s1 = 0.5 * (p1 + 1.0) * viewportSize;
    
    // Line direction and perpendicular in screen space
    vec2 dir = normalize(s1 - s0);
    vec2 perp = vec2(-dir.y, dir.x);
    
    // Apply perspective-corrected width
    float halfWidth0 = width0 * 0.5;
    float halfWidth1 = width1 * 0.5;
    
    // Create offset points in screen space
    vec2 o0a = s0 + perp * halfWidth0;
    vec2 o0b = s0 - perp * halfWidth0;
    vec2 o1a = s1 + perp * halfWidth1;
    vec2 o1b = s1 - perp * halfWidth1;
    
    // Convert back to NDC, then to clip space
    vec2 n0a = (o0a / viewportSize) * 2.0 - 1.0;
    vec2 n0b = (o0b / viewportSize) * 2.0 - 1.0;
    vec2 n1a = (o1a / viewportSize) * 2.0 - 1.0;
    vec2 n1b = (o1b / viewportSize) * 2.0 - 1.0;
    
    // Emit triangle strip forming a quad
    gl_Position = vec4(n0a * start.w, start.z, start.w); v_color = vs_color[0]; EmitVertex();
    gl_Position = vec4(n0b * start.w, start.z, start.w); v_color = vs_color[0]; EmitVertex();
    gl_Position = vec4(n1a * end.w, end.z, end.w); v_color = vs_color[1]; EmitVertex();
    gl_Position = vec4(n1b * end.w, end.z, end.w); v_color = vs_color[1]; EmitVertex();
    
    EndPrimitive();
}