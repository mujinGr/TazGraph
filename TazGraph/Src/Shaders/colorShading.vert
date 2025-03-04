#version 330

layout (location = 0) in vec3 vertexPosition; //vec2 is array of 2 floats
layout (location = 1) in vec4 vertexColor;
//layout (location = 2) in vec3 centerPosition;

out vec4 fragmentColor;

uniform mat4 projection;

uniform mat4 rotationMatrix;
uniform vec3 centerPosition;

//uniform mat4 translation;


///// FUNCTIONS /////



mat4 BuildTranslation(vec3 delta)
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(delta, 1.0));
}



/////   MAIN  /////


void main() {
    mat4 translationINV = BuildTranslation(-centerPosition);
    mat4 translationOG  = BuildTranslation(centerPosition);

    //gl_Position = projection * translation * rotationMatrix * inverseTranslation * vec4(vertexPosition.xyz, 1.0);
    gl_Position = projection * translationOG * rotationMatrix * translationINV      * vec4(vertexPosition.xyz, 1.0);

    fragmentColor = vertexColor;
}