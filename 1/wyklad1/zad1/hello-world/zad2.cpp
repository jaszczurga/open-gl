// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// -------------------------------------------------
// Hello World w OpenGL
// -------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>

// Pliki naglowkoweg OpenGLa
#include <GL/glew.h>
#include <GL/freeglut.h>


// Identyfikatory obiektow
GLuint idProgram;	// potoku/programu
GLuint idVBO;		// bufora wierzcholkow
GLuint idVAO;		// tablic wierzcholkow

const int N = 100;


// Naglowki naszych funkcji
void DisplayScene();
void Initialize();
void CreateVertexShader( void );
void CreateFragmentShader( void );
void Reshape( int width, int height );




// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// ---------------------------------------
	// Etap (1) utworzenie kontekstu OpenGL,
	// okna aplikacji oraz rejestracja funkcji
	// zwrotnych (za pomoca freeGlut)
	// ---------------------------------------

	// freeGLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitContextVersion( 3, 3 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 500, 500 );
	glutCreateWindow( "Hello World w OpenGL!" );

	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );


	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err )
	{
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if( !GLEW_VERSION_3_3 )
	{
		printf("Brak OpenGL 3.3!\n");
		exit(1);
	}

	// Stworzenie obiektow OpenGLa
	// buforow, potoku, przekazanie danych
	// do pamieci karty graficznej
	Initialize();


	// ---------------------------
	// Etap (5) rendering
	// ---------------------------
	glutMainLoop();




	// Cleaning
	glDeleteProgram( idProgram );
	glDeleteBuffers( 1, &idVBO );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}

// ---------------------------------------
void DisplayScene()
{
	// ---------------------------
	// Etap (5) rendering
	// ---------------------------

	// 1. czyszczenie bufora ramki koloru
	glClear( GL_COLOR_BUFFER_BIT );


	// 2. aktywowanie programu
	glUseProgram( idProgram );

		// 3. wlaczenie obiektu tablic wierzcholkow
		glBindVertexArray( idVAO );

		// 4. uruchomienie aktualnego programu na aktualnej tablicy wierzcholkow
		glDrawArrays( GL_TRIANGLES, 0, N*3 );

		// 5. wylaczenie obiektu tablic wierzcholkow
		glBindVertexArray( 0 );


	// 6. wylaczenie programu
	glUseProgram( 0 );


	// 7. wyswietlanie danych w oknie aplikacji
	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
	// ---------------------------------------
	// Etap (2) przeslanie danych geometrii do OpenGL
	// ---------------------------------------

	// Stworzenie identyfikatora obiektu oraz obiektu tablic wierzcholkow
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

	// Generowanie identyfikatora obiektu oraz obiektu bufora
	glGenBuffers( 1, &idVBO );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO );

	srand(time(NULL));

	GLfloat* triangles = new GLfloat[N*3*2];

	for(int i = 0;i < N; i++){
		
		float centerX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        float centerY = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

		float size = ((float)rand() / RAND_MAX) * 0.2f + 0.05f;

		float angle = ((float)rand() / RAND_MAX) * 3.14159f * 2.0f;

		for(int j = 0; j < 3; j++)
        {
            float vertexAngle = angle + (j * 2.0f * 3.14159f / 3.0f);
            int idx = (i * 3 + j) * 2;
            triangles[idx]     = centerX + size * cos(vertexAngle);
            triangles[idx + 1] = centerY + size * sin(vertexAngle);
        }
	}


	// Zaladowanie danych do obiektu bufora wierzcholkow
	glBufferData( GL_ARRAY_BUFFER, sizeof(float)*N*3*2, triangles, GL_STATIC_DRAW );

	// Position attribute (first 2 floats of each vertex)
glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0 );
glEnableVertexAttribArray( 0 );


	// wylaczenie obiektu tablic wierzcholkow
	glBindVertexArray( 0 );

	delete[] triangles;

	// ---------------------------------------
	// Etap (3) utworzenie potoku graficznego
	// ---------------------------------------

	idProgram = glCreateProgram();
	CreateVertexShader();
	CreateFragmentShader();

	// Walidacja programu
	glLinkProgram( idProgram );
	glValidateProgram( idProgram );


	// -----------------------------------------
	// Etap (4) ustawienie maszyny stanow OpenGL
	// -----------------------------------------

	// ustawianie koloru ktorym bedzie czyszczony bufor ramki
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

// ---------------------------------------
void CreateVertexShader( void )
{
	GLuint shader = glCreateShader( GL_VERTEX_SHADER );

	const GLchar * code =
	"#version 330 core \n		"
	"in vec4 inPosition;		"
	"void main() 				"
	"{ 							"
	"	gl_Position = inPosition; "
	"}							";

	glShaderSource( shader, 1, &code, NULL );
	glCompileShader( shader );

	GLint status;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
	if( status != GL_TRUE )
	{
		printf("Blad kompilacji vertex shadera!\n");
		exit(1);
	}
	glAttachShader( idProgram, shader );
}

// ---------------------------------------
void CreateFragmentShader( void )
{
	GLuint shader = glCreateShader( GL_FRAGMENT_SHADER );

	
    const GLchar *code =
    "#version 330 core\n"
    "out vec4 outColor;\n"
    "void main()\n"
    "{\n"
    "	float primitiveR = sin(float(gl_PrimitiveID) * 0.5) * 0.5 + 0.5;\n"
    "	float primitiveG = sin(float(gl_PrimitiveID) * 0.7) * 0.5 + 0.5;\n"
    "	float primitiveB = sin(float(gl_PrimitiveID) * 0.9) * 0.5 + 0.5;\n"
    "	\n"
    "	float fragR = gl_FragCoord.x / 500.0;\n"
    "	float fragG = gl_FragCoord.y / 500.0;\n"
    "	\n"
    "	vec3 color = vec3(primitiveR * 0.7 + fragR * 0.3,\n"
    "	                   primitiveG * 0.7 + fragG * 0.3,\n"
    "	                   primitiveB);\n"
    "	\n"
    "	outColor = vec4(color, 1.0);\n"
    "}\n";


	glShaderSource( shader, 1, &code, NULL );
	glCompileShader( shader );

	GLint status;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
	if( status != GL_TRUE )
	{
		printf("Blad kompilacji fragment shadera!\n");
		exit(1);
	}
	glAttachShader( idProgram, shader );
}

// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
}
