// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// Refactored version with Scene architecture
// -------------------------------------------------
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "SceneManager.hpp"
#include "RandomSquaresScene.hpp"
#include "GradientSquaresScene.hpp"
#include "HouseScene.hpp"

// Global scene manager
SceneManager* sceneManager = nullptr;

// ---------------------------------------
// GLUT Callback Functions
// ---------------------------------------

void DisplayScene()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (sceneManager) {
        sceneManager->DisplayCurrentScene();
    }
    
    glutSwapBuffers();
}

void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Keyboard(unsigned char key, int x, int y)
{
    bool shouldRedisplay = false;
    
    // Let scene manager handle scene-specific keys
    if (sceneManager && sceneManager->HandleKeyboard(key, x, y)) {
        shouldRedisplay = true;
    }
    
    // Handle global keys
    switch(key)
    {
        case 27:    // ESC key
        case 'x':
            glutLeaveMainLoop();
            break;
            
        case ' ':  // Space key - switch scenes
            if (sceneManager) {
                sceneManager->NextScene();
                shouldRedisplay = true;
            }
            break;
    }
    
    if (shouldRedisplay) {
        glutPostRedisplay();
    }
}

// ---------------------------------------
// Initialization
// ---------------------------------------

void Initialize()
{
    // Create scene manager
    sceneManager = new SceneManager();
    
    // Create scenes
    RandomSquaresScene* scene1 = new RandomSquaresScene();
    GradientSquaresScene* scene2 = new GradientSquaresScene();
    HouseScene* scene3 = new HouseScene();
    
    // Add scenes to manager
    sceneManager->AddScene(scene1);
    sceneManager->AddScene(scene2);
    sceneManager->AddScene(scene3);
    
    // Initialize all scenes
    scene1->Initialize();
    scene2->Initialize(scene1->GetCoordBuffer()); // Share coordinate buffer
    scene3->Initialize();
    
    // Set clear color
    glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
    
    printf("\n=== OpenGL Scene Manager ===\n");
    printf("Controls:\n");
    printf("  SPACE - Switch between scenes\n");
    printf("  W/A/S/D - Move house (Scene 3 only)\n");
    printf("  ESC/X - Exit\n");
    printf("============================\n\n");
}

// ---------------------------------------
// Main
// ---------------------------------------

int main(int argc, char *argv[])
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(500, 500);
    glutCreateWindow("OpenGL - Scene Manager");
    
    // Set callbacks
    glutDisplayFunc(DisplayScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        printf("GLEW Error\n");
        exit(1);
    }
    
    // Check OpenGL version
    if(!GLEW_VERSION_3_3) {
        printf("Brak OpenGL 3.3!\n");
        exit(1);
    }
    
    // Initialize scenes
    Initialize();
    
    // Start main loop
    glutMainLoop();
    
    // Cleanup
    delete sceneManager;
    
    return 0;
}
