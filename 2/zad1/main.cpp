// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// -------------------------------------------------
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib> // dla rand()
#include <ctime>   // dla time()
#include <cmath>

// Funkcje pomocnicze
#include "utilities.hpp"


// ---------------------------------------
// Identyfikatory obiektow OpenGL dla Sceny 1

GLuint idProgram1;    // Program/potok dla Sceny 1
GLuint idVAO1;        // Tablica wierzcholkow dla Sceny 1
GLuint idVBO_coord1;  // Bufor na wspolrzedne dla Sceny 1
GLuint idVBO_color1;  // Bufor na kolory dla Sceny 1

// Identyfikatory obiektow OpenGL dla Sceny 2
GLuint idProgram2;    // Program/potok dla Sceny 2
GLuint idVAO2;        // Tablica wierzcholkow dla Sceny 2
// koordynaty uzywamy ze sceny 1
GLuint idVBO_color2;  // NOWY bufor na kolory (dla gradientu)

// Identyfikatory obiektow OpenGL dla Sceny 3
GLuint idProgram3;
GLuint idVAO3;
GLuint idVBO_coord3; // Tylko jeden bufor, bo kolory sa w shaderze

// Dane domku
#include "house.h" // Dzieki temu mamy NUMBER_OF_VERTICES i Mesh_Vertices

// Zmienne do sterowania ruchem domku (zmienne jednorodne)
GLfloat offsetX = 0.0f;
GLfloat offsetY = 0.0f;
const GLfloat MOVE_SPEED = 0.05f;

// Lokalizacje zmiennych jednorodnych
GLint uLocation_offset;

// Dane globalne
const int N = 100; // Zdefiniowane N w jednym miejscu
const int NUM_VERTICES_PER_SQUARE = 6; // Kwadrat jako dwa trojkaty
const int TOTAL_VERTICES1 = N * NUM_VERTICES_PER_SQUARE;

// Zarządzanie scenami
enum SceneType { SCENE_1, SCENE_2, SCENE_3 };
SceneType currentScene = SCENE_1; 

// Deklaracje funkcji specyficznych dla sceny
void InitializeScene1();
void DisplayScene1();

// Deklaracje funkcji specyficznych dla sceny
void InitializeScene2();
void DisplayScene2();

// Deklaracje funkcji specyficznych dla sceny
void InitializeScene3();
void DisplayScene3();

// ---------------------------------------
// Funkcje pomocnicze do generowania danych

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
}

// ---------------------------------------
// Funkcja generujaca dane kolorow dla Sceny 2 (z gradientami)
void GenerateScene2ColorData(GLfloat** colors)
{
    *colors = new GLfloat[TOTAL_VERTICES1 * 3]; // 3 skladowe (r, g, b) na wierzcholek

    // Uzywamy tego samego inicjatora losowosci co dla Sceny 1, 
    // aby kolory bazowe byly losowe, ale wierzcholki mialy rozne kolory.
    srand(time(0)); 

    for (int i = 0; i < N; ++i)
    {
        // Generujemy 4 rozne bazowe kolory dla 4 rogów kwadratu
        float R[4], G[4], B[4];
        for (int k = 0; k < 4; ++k) {
            R[k] = randomFloat(0.2f, 1.0f);
            G[k] = randomFloat(0.2f, 1.0f);
            B[k] = randomFloat(0.2f, 1.0f);
        }

        // Indeksy kolorow dla 4 wierzcholkow logicznych:
        // C0 (dol-lewy), C1 (dol-prawy), C2 (gora-prawy), C3 (gora-lewy)
        // C0 = (R[0], G[0], B[0]), itd.

        // Indeksowanie w buforze
        int baseColorIndex = i * 18; // 6 wierzcholkow * 3 skladowe
        
        // Kwadrat jako dwa trójkąty: T1 (W0, W1, W3) i T2 (W1, W2, W3)

        // Trójkąt 1: W0, W1, W3
        // W0 (dol-lewy, C0)
        (*colors)[baseColorIndex + 0] = R[0]; (*colors)[baseColorIndex + 1] = G[0]; (*colors)[baseColorIndex + 2] = B[0];
        // W1 (dol-prawy, C1)
        (*colors)[baseColorIndex + 3] = R[1]; (*colors)[baseColorIndex + 4] = G[1]; (*colors)[baseColorIndex + 5] = B[1];
        // W3 (gora-lewy, C3)
        (*colors)[baseColorIndex + 6] = R[3]; (*colors)[baseColorIndex + 7] = G[3]; (*colors)[baseColorIndex + 8] = B[3];
        
        // Trójkąt 2: W1, W2, W3
        // W1 (dol-prawy, C1)
        (*colors)[baseColorIndex + 9] = R[1]; (*colors)[baseColorIndex + 10] = G[1]; (*colors)[baseColorIndex + 11] = B[1];
        // W2 (gora-prawy, C2)
        (*colors)[baseColorIndex + 12] = R[2]; (*colors)[baseColorIndex + 13] = G[2]; (*colors)[baseColorIndex + 14] = B[2];
        // W3 (gora-lewy, C3)
        (*colors)[baseColorIndex + 15] = R[3]; (*colors)[baseColorIndex + 16] = G[3]; (*colors)[baseColorIndex + 17] = B[3];
    }
}

// ---------------------------------------
void InitializeScene2()
//moglismy wykorzystac potok ze sceny 1 ale musielismy stworzyc nowy fragment shader
{
    GLfloat *colors;
    GenerateScene2ColorData(&colors); // Generowanie danych kolorow

    // 1. Tworzymy program (potok)
    idProgram2 = glCreateProgram();
    glAttachShader( idProgram2, LoadShader(GL_VERTEX_SHADER, "vertex.glsl")); 
    glAttachShader( idProgram2, LoadShader(GL_FRAGMENT_SHADER, "fragment2.glsl")); 
    LinkAndValidateProgram( idProgram2 );

    // 2. Konfiguracja VAO/VBO
    glGenVertexArrays( 1, &idVAO2 );
    glBindVertexArray( idVAO2 );

        // REUŻYCIE: Bufor na wspolrzedne z Sceny 1 (Atrybut 0)
        // Nie tworzymy nowego bufora, tylko używamy już istniejącego idVBO_coord1.
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord1 );
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 0 );

        // NOWY: Bufor na kolory wierzcholkow (Atrybut 1)
        glGenBuffers( 1, &idVBO_color2 );
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_color2 );
        glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * TOTAL_VERTICES1 * 3, colors, GL_STATIC_DRAW );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 1 );

    // Odwiązanie VAO
    glBindVertexArray( 0 );
    
    // Zwolnienie pamięci RAM
    delete[] colors;

    printf("Scena 2 zainicjalizowana: %d kwadratow z gradientem.\n", N);
}

// ---------------------------------------
void InitializeScene3()
{
    // 1. Tworzymy program (potok) dla Sceny 3
    idProgram3 = glCreateProgram();
    glAttachShader( idProgram3, LoadShader(GL_VERTEX_SHADER, "vertex3.glsl")); // NOWY VERTEX SHADER!
    glAttachShader( idProgram3, LoadShader(GL_FRAGMENT_SHADER, "fragment3.glsl")); // NOWY FRAGMENT SHADER!
    LinkAndValidateProgram( idProgram3 );

    // Pobranie lokalizacji zmiennej jednorodnej "uOffset"
    glUseProgram(idProgram3);
    uLocation_offset = glGetUniformLocation(idProgram3, "uOffset");
    glUseProgram(0);

    // 2. Konfiguracja VAO/VBO
    glGenVertexArrays( 1, &idVAO3 );
    glBindVertexArray( idVAO3 );

        // Bufor na wspolrzedne domku (Atrybut 0)
        glGenBuffers( 1, &idVBO_coord3 );
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord3 );
        // Uwaga: Mesh_Vertices to plaska tablica, ktora zawiera x, y, x, y...
        glBufferData( GL_ARRAY_BUFFER, sizeof( Mesh_Vertices ), Mesh_Vertices, GL_STATIC_DRAW ); 
        
        // Atrybut 0: wspolrzedne (vec2)
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 0 );

        // W tej scenie NIE AKTYWUJEMY Atrybutu 1 (kolory), 
        // bo Vertex Shader go nie potrzebuje!

    // Odwiązanie VAO
    glBindVertexArray( 0 );

    printf("Scena 3 zainicjalizowana: domek z Blendra.\n");
}

void GenerateScene1Data(GLfloat** coords, GLfloat** colors)
{
    // Alokacja pamieci
    *coords = new GLfloat[TOTAL_VERTICES1 * 2]; // 2 skladowe (x, y) na wierzcholek
    *colors = new GLfloat[TOTAL_VERTICES1 * 3]; // 3 skladowe (r, g, b) na wierzcholek

    srand(time(0)); // Inicjalizacja generatora liczb losowych

    for (int i = 0; i < N; ++i)
    {
        // 1. Losowa pozycja srodka kwadratu i rozmiar
        float cx = randomFloat(-0.9f, 0.9f);
        float cy = randomFloat(-0.9f, 0.9f);
        float size = randomFloat(0.05f, 0.15f); 
        float hSize = size / 2.0f;

        // 2. Losowy kolor dla calego kwadratu (staly kolor)
        float r = randomFloat(0.1f, 1.0f);
        float g = randomFloat(0.1f, 1.0f);
        float b = randomFloat(0.1f, 1.0f);

        // Indeksowanie w buforach
        int baseCoordIndex = i * 12; // 6 wierzcholkow * 2 skladowe
        int baseColorIndex = i * 18; // 6 wierzcholkow * 3 skladowe
        
        // Kwadrat jako dwa trójkąty: T1 (W0, W1, W3) i T2 (W1, W2, W3)
        // Wierzchołki: W0(dol-lewy), W1(dol-prawy), W2(gora-prawy), W3(gora-lewy)

        // Trójkąt 1 (W0, W1, W3)
        // W0
        (*coords)[baseCoordIndex + 0] = cx - hSize; (*coords)[baseCoordIndex + 1] = cy - hSize;
        // W1
        (*coords)[baseCoordIndex + 2] = cx + hSize; (*coords)[baseCoordIndex + 3] = cy - hSize;
        // W3
        (*coords)[baseCoordIndex + 4] = cx - hSize; (*coords)[baseCoordIndex + 5] = cy + hSize;
        
        // Trójkąt 2 (W1, W2, W3)
        // W1
        (*coords)[baseCoordIndex + 6] = cx + hSize; (*coords)[baseCoordIndex + 7] = cy - hSize;
        // W2
        (*coords)[baseCoordIndex + 8] = cx + hSize; (*coords)[baseCoordIndex + 9] = cy + hSize;
        // W3
        (*coords)[baseCoordIndex + 10] = cx - hSize; (*coords)[baseCoordIndex + 11] = cy + hSize;


        // Kolory (staly kolor dla wszystkich 6 wierzcholkow)
        for(int j = 0; j < 6; ++j) {
            int colorIndex = baseColorIndex + j * 3;
            (*colors)[colorIndex + 0] = r;
            (*colors)[colorIndex + 1] = g;
            (*colors)[colorIndex + 2] = b;
        }
    }
}

// ---------------------------------------
// Implementacje scen

void InitializeScene1()
{
    GLfloat *coords, *colors;
    GenerateScene1Data(&coords, &colors); // Generowanie danych

    // 1. Tworzymy program (potok)
    idProgram1 = glCreateProgram();
    glAttachShader( idProgram1, LoadShader(GL_VERTEX_SHADER, "vertex.glsl")); 
    glAttachShader( idProgram1, LoadShader(GL_FRAGMENT_SHADER, "fragment1.glsl"));

    LinkAndValidateProgram( idProgram1 );

    // 2. Konfiguracja VAO/VBO
    glGenVertexArrays( 1, &idVAO1 );
    glBindVertexArray( idVAO1 );

        // Bufor na wspolrzedne wierzcholkow (Atrybut 0)
        glGenBuffers( 1, &idVBO_coord1 );
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord1 );
        glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * TOTAL_VERTICES1 * 2, coords, GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 0 );

        // Bufor na kolory wierzcholkow (Atrybut 1)
        glGenBuffers( 1, &idVBO_color1 );
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_color1 );
        glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * TOTAL_VERTICES1 * 3, colors, GL_STATIC_DRAW );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 1 );

    // Odwiązanie VAO
    glBindVertexArray( 0 );
    
    // Zwolnienie pamięci RAM
    delete[] coords;
    delete[] colors;

    printf("Scena 1 zainicjalizowana: %d kwadratow.\n", N);
}

void DisplayScene1()
{
    // Wlaczenie potoku dla Sceny 1
    glUseProgram( idProgram1 );

        // Aktywacja tablicy wierzcholkow (VAO1)
        glBindVertexArray( idVAO1 );
        
        // Renderowanie 6 wierzcholkow na kazdy z N kwadratow
        glDrawArrays( GL_TRIANGLES, 0, TOTAL_VERTICES1 );
        
        // Dezaktywujemy VAO
        glBindVertexArray( 0 );

    // Dezaktywujemy potok
    glUseProgram( 0 );
}

// ---------------------------------------
void DisplayScene2()
{
    // Wlaczenie potoku dla Sceny 2
    glUseProgram( idProgram2 );

        // Aktywacja tablicy wierzcholkow (VAO2)
        glBindVertexArray( idVAO2 );
        
        // Renderowanie 6 wierzcholkow na kazdy z N kwadratow
        glDrawArrays( GL_TRIANGLES, 0, TOTAL_VERTICES1 );
        
        // Dezaktywujemy VAO
        glBindVertexArray( 0 );

    // Dezaktywujemy potok
    glUseProgram( 0 );
}

// ---------------------------------------
void DisplayScene3()
{
    // Wlaczenie potoku dla Sceny 3
    glUseProgram( idProgram3 );

        // PRZESYŁANIE ZMIENNYCH JEDNORODNYCH
        // Przesylamy aktualny wektor przesuniecia do shadera
        glUniform2f(uLocation_offset, offsetX, offsetY);

        // Aktywacja tablicy wierzcholkow (VAO3)
        glBindVertexArray( idVAO3 );
        
        // Renderowanie wszystkich trojkatow domku
        glDrawArrays( GL_TRIANGLES, 0, NUMBER_OF_VERTICES );
        
        // Dezaktywujemy VAO
        glBindVertexArray( 0 );

    // Dezaktywujemy potok
    glUseProgram( 0 );
}

// ---------------------------------------
// Glowna funkcja rysujaca
void DisplayScene()
{
    // ---------------------------
    // Etap (5) rendering
    // ---------------------------
    glClear( GL_COLOR_BUFFER_BIT );

    switch (currentScene) {
        case SCENE_1:
            DisplayScene1();
            break;
        case SCENE_2:
            DisplayScene2();
            break;
        case SCENE_3:
            DisplayScene3();
            break;
    }


    glutSwapBuffers();
}

// ---------------------------------------
// Glowna funkcja inicjalizujaca
void Initialize()
{
    // Inicjalizacja wszystkich scen
    InitializeScene1(); 
    InitializeScene2();
    InitializeScene3();

    // ustawienie koloru czyszczenia ramki koloru
    glClearColor( 0.9f, 0.9f, 0.9f, 0.9f );

}

// ---------------------------------------------------
// Funkcja wywolywana podczas tworzenia okna aplikacji
void Reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
}



// ---------------------------------------------------
// Funkcja wywolywana podczas wcisniecia klawisza ASCII
void Keyboard( unsigned char key, int x, int y )
{
    bool shouldRedisplay = false;

    // 1. OBSŁUGA STEROWANIA WSAD (Tylko w Scenie 3)
    if (currentScene == SCENE_3) {
        switch (key) {
            case 'w': case 'W':
                offsetX -= MOVE_SPEED;
                shouldRedisplay = true;
                break;
            case 's': case 'S':
                offsetX += MOVE_SPEED;
                shouldRedisplay = true;
                break;
            case 'a': case 'A':
                offsetY -= MOVE_SPEED;
                shouldRedisplay = true;
                break;
            case 'd': case 'D':
                offsetY += MOVE_SPEED;
                shouldRedisplay = true;
                break;
        }
    }

    // 2. OBSŁUGA KLAWISZY GLOBALNYCH (ESC, SPACE, X)
    switch(key)
    {
        case 27:    // ESC key
            glutLeaveMainLoop();
            break;

        case 'x':
            glutLeaveMainLoop();
            break;

        case ' ':  // Space key
            // Logika przełączania sceny
            switch (currentScene) {
                case SCENE_1: currentScene = SCENE_2; printf("Przebaczono na SCENE 2\n"); break;
                case SCENE_2: currentScene = SCENE_3; printf("Przebaczono na SCENE 3\n"); break;
                case SCENE_3: currentScene = SCENE_1; printf("Przebaczono na SCENE 1\n"); break;
            }
            shouldRedisplay = true; // Zmiana sceny wymusza przerysowanie
            break;
    }

    // 3. Wymuszenie przerysowania, jeśli nastąpiła zmiana
    if (shouldRedisplay) {
        glutPostRedisplay();
    }
}

// ---------------------------------------------------
int main( int argc, char *argv[] )
{
    // ... (kod inicjalizacyjny glut/glew jak w oryginalnym pliku)
    
    // GLUT
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
    glutInitContextVersion( 3, 3 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutInitWindowSize( 500, 500 );
    glutCreateWindow( "Zadanie OpenGL - Sceny" );
    glutDisplayFunc( DisplayScene );
    glutReshapeFunc( Reshape );
    glutKeyboardFunc( Keyboard );


    // GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if( GLEW_OK != err ) {
        printf("GLEW Error\n");
        exit(1);
    }

    // OpenGL
    if( !GLEW_VERSION_3_3 ) {
        printf("Brak OpenGL 3.3!\n");
        exit(1);
    }


    // Inicjalizacja
    Initialize();

    // Rendering
    glutMainLoop();


    glDeleteProgram( idProgram1 );
glDeleteBuffers( 1, &idVBO_coord1 ); 
glDeleteBuffers( 1, &idVBO_color1 );
glDeleteVertexArrays( 1, &idVAO1 );

glDeleteProgram( idProgram2 );
glDeleteBuffers( 1, &idVBO_color2 );
glDeleteVertexArrays( 1, &idVAO2 );

glDeleteProgram( idProgram3 );
glDeleteBuffers( 1, &idVBO_coord3 );
glDeleteVertexArrays( 1, &idVAO3 );

    return 0;
}