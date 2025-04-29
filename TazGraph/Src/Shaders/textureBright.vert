#version 330

layout (location = 0) in vec3 vertexPosition; //vec2 is array of 2 floats
layout (location = 1) in vec4 vertexColor;
layout (location = 2) in vec3 vertexRotation;
layout (location = 3) in vec2 vertexUV;

out vec4 fragmentColor;
out vec2 fragmentUV;


uniform vec3 centerPosition;

uniform mat4 projection;


///// FUNCTIONS /////

mat4 BuildInstanceRotation(vec3 rotationAngles) {
    float cx = cos(rotationAngles.x);
    float sx = sin(rotationAngles.x);
    float cy = cos(rotationAngles.y);
    float sy = sin(rotationAngles.y);
    float cz = cos(rotationAngles.z);
    float sz = sin(rotationAngles.z);

    mat4 rotX = mat4(
        vec4(1, 0, 0, 0),
        vec4(0, cx, -sx, 0),
        vec4(0, sx, cx, 0),
        vec4(0, 0, 0, 1)
    );

    mat4 rotY = mat4(
        vec4(cy, 0, sy, 0),
        vec4(0, 1, 0, 0),
        vec4(-sy, 0, cy, 0),
        vec4(0, 0, 0, 1)
    );

    mat4 rotZ = mat4(
        vec4(cz, -sz, 0, 0),
        vec4(sz, cz, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(0, 0, 0, 1)
    );

    return rotZ * rotY * rotX; // Apply in ZYX order (common)
}

/////// MAIN ////

void main() {

    mat4 localRotation = BuildInstanceRotation(vertexRotation);

    mat4 translation = mat4(1.0);
    translation[3].xyz = centerPosition;

    gl_Position = projection * translation * localRotation * vec4(vertexPosition.xyz, 1.0);

    fragmentColor = vertexColor;

    fragmentUV = vertexUV;
}