// vertex_scene3.glsl
#version 330 core

// Atrybut wierzcholka (wspolrzedne - location 0)
layout (location = 0) in vec4 inPosition;

// Zmienna jednorodna przeslana z C++ do przesuniecia (XY)
uniform vec2 uOffset;

// Zmienne wyjsciowe dla Fragment Shadera
flat out int vPrimitiveID; // Przekazujemy ID prymitywu
out vec4 vPosition;   // Przekazujemy pozycję wierzchołka

void main()
{
vec4 finalPosition = inPosition + vec4(uOffset, 0.0, 0.0);

    //(x, y) -> (y, -x)
    vec4 rotatedPosition;
    rotatedPosition.x = finalPosition.y;
    rotatedPosition.y = -finalPosition.x;
    rotatedPosition.z = finalPosition.z; 
    rotatedPosition.w = finalPosition.w;

 vPrimitiveID = gl_VertexID; 
 vPosition = rotatedPosition; 
 gl_Position = rotatedPosition; 
}






















