// fragment_scene3.glsl
#version 330 core

// Odbieramy ID prymitywu z Vertex Shadera
flat in int vPrimitiveID;
// Odbieramy pozycję wierzchołka (po transformacji)
in vec4 vPosition; 

out vec4 outColor;

void main()
{
    // Wbudowana zmienna: gl_FragCoord to wspolrzedne okna piksela (x, y, z, w)
    // uzyjemy gl_FragCoord.xy do generowania gradientu
    vec2 screenUV = gl_FragCoord.xy / 500.0; // Normalizujemy do zakresu okna (zakładamy 500x500)
    
    // Zmienne bazowe dla losowego/rozróżniającego koloru
    float r_base = mod(float(vPrimitiveID) * 0.35, 1.0); // Baza R zalezna od ID trojkata
    float g_base = mod(float(vPrimitiveID) * 0.70, 1.0); // Baza G
    float b_base = mod(float(vPrimitiveID) * 0.15, 1.0); // Baza B

    // 1. Ustawienie koloru bazowego (różne trójkąty = różne kolory)
    vec3 baseColor = vec3(r_base, g_base, b_base);

    // 2. Dodanie efektu przejścia gradientowego
    // Gradient poziomy (zalezny od X okna)
    float gradientX = sin(screenUV.x * 3.14159 * 2.0) * 0.2 + 0.8; // Oscylacja dla efektu
    
    // Gradient pionowy (zalezny od Y okna)
    float gradientY = (gl_FragCoord.y / 500.0); // Prosty gradient od dołu (0.0) do góry (1.0)
    
    // Mieszanie koloru bazowego z gradientem i dodanie szumu
    vec3 finalColor = baseColor * gradientX * gradientY;
    
    // // Dodatkowy efekt "ożywienia" (np. mruganie światła)
    // // Uzywamy modyfikatora odleglosci od srodka domku w pozycji X
    float distMod = abs(vPosition.x);
    finalColor.r += distMod * 0.2; // Lekka modyfikacja koloru czerwonego
    finalColor.g += sin(vPosition.x * 5.0) * 0.1;
    finalColor.b += cos(vPosition.x * 3.0) * 0.1;

    // 3. Finalny kolor
    outColor = vec4(finalColor, 1.0);
}






