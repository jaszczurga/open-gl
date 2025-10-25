// -------------------------------------------------
// Base Scene Interface
// -------------------------------------------------
#ifndef __SCENE_HPP
#define __SCENE_HPP

#include <GL/glew.h>
#include <GL/freeglut.h>

// Abstract base class for all scenes
class Scene {
public:
    virtual ~Scene() {}
    
    // Initialize the scene (create shaders, VAOs, VBOs, etc.)
    virtual void Initialize() = 0;
    
    // Render the scene
    virtual void Display() = 0;
    
    // Handle keyboard input (return true if handled)
    virtual bool HandleKeyboard(unsigned char key, int x, int y) = 0;
    
    // Clean up resources
    virtual void Cleanup() = 0;
    
    // Get scene name for debugging
    virtual const char* GetName() const = 0;
};

#endif
