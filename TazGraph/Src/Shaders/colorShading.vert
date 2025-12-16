#version 430

layout (location = 0) in vec3 aPos; //vertex Pos

layout (location = 1) in vec3 instanceSize;
layout (location = 2) in vec3 instanceBodyCenter;
layout (location = 3) in vec3 instanceRotation;
layout (location = 4) in vec4 vertexColor;


out vec4 fragmentColor;

uniform mat4 u_ViewProjection;

uniform mat4 rotationMatrix;

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


/////   MAIN  /////
void main() {
    vec3 scaledPos = aPos * instanceSize;
    
    mat4 localRotation = BuildInstanceRotation(instanceRotation);

    mat4 translation = mat4(1.0);
    translation[3].xyz = instanceBodyCenter;

    gl_Position = u_ViewProjection * translation * rotationMatrix * localRotation * vec4(scaledPos.xyz, 1.0);

    fragmentColor = vertexColor;
}