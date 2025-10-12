// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// -------------------------------------------------
// Hello World w OpenGL
// -------------------------------------------------
#include <stdio.h>

// Pliki naglowkoweg OpenGLa
#include <GL/glew.h>
#include <GL/freeglut.h>


// Identyfikatory obiektow
GLuint idProgram;	// potoku/programu
GLuint idVBO;		// bufora wierzcholkow
GLuint idVAO;		// tablic wierzcholkow


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
		glDrawArrays( GL_TRIANGLES, 0, 2*3 );

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

	// Wspolrzedne wierzcholkow
	GLfloat triangles[2*3*5] =  // 5 values per vertex: X, Y, Red, Green, Blue
{
    // First triangle (GREEN)
    -1.f, -1.f,   0.f, 1.f, 1.f,  // bottom-left: green
     1.f, -1.f,   0.1f, 0.5f, 0.f,  // bottom-right: green
     0.0f,  0.0f,   1.f, 1.f, 0.3f,  // top: green
    
    // Second triangle (RED)
    -0.7f,  -0.3f,  1.f, 1.f, 0.f,  // bottom-left: red
     1.0f,  0.f,  1.f, 0.f, 0.f,  // bottom-right: red
     0.5f,  1.0f, 1.f, 0.5f, 0.f   // top: red
};

	// Zaladowanie danych do obiektu bufora wierzcholkow
	glBufferData( GL_ARRAY_BUFFER, sizeof(float)*30, triangles, GL_STATIC_DRAW );

	// Position attribute (first 2 floats of each vertex)
glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0 );
glEnableVertexAttribArray( 0 );

// Color attribute (next 3 floats of each vertex)
glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)) );
glEnableVertexAttribArray( 1 );

	// wylaczenie obiektu tablic wierzcholkow
	glBindVertexArray( 0 );


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
	"in vec3 inColor;           "  // NEW: receive color
"out vec3 fragColor;        "  // NEW: pass to fragment shader
	"void main() 				"
	"{ 							"
	"	gl_Position = inPosition; "
	"   fragColor = inColor;    "
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
	"#version 150 core \n		"
	"in vec3 fragColor;         "  // NEW: receive color from vertex shader
	"out vec4 outColor;			"
	"void main()				"
	"{							"
	"	outColor = vec4( fragColor, 1.f ); "  // NEW: use passed color
	"}";


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
