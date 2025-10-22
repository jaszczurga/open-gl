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
 // 1. Zastosowanie przesuniecia
vec4 finalPosition = inPosition + vec4(uOffset, 0.0, 0.0);

    // 2. Obrót o 90 stopni w prawo (x, y) -> (y, -x)
    // Tworzymy nowy wektor dla obróconej pozycji
    vec4 rotatedPosition;
    rotatedPosition.x = finalPosition.y;
    rotatedPosition.y = -finalPosition.x;
    rotatedPosition.z = finalPosition.z; // Zostawiamy Z bez zmian
    rotatedPosition.w = finalPosition.w; // Zostawiamy W bez zmian

// 3. Przekazanie wbudowanych zmiennych
 vPrimitiveID = gl_VertexID; 
vPosition = rotatedPosition; // Używamy nowej, obróconej pozycji
// 4. Finalna pozycja
gl_Position = rotatedPosition; // Używamy nowej, obróconej pozycji
}






















