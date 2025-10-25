// -------------------------------------------------
// Scene 3: Moving House
// -------------------------------------------------
#ifndef __HOUSE_SCENE_HPP
#define __HOUSE_SCENE_HPP

#include "Scene.hpp"
#include "utilities.hpp"
#include "house.h"
#include <cstdio>

class HouseScene : public Scene {
private:
    GLuint idProgram;
    GLuint idVAO;
    GLuint idVBO_coord;
    
    // Movement variables
    GLfloat offsetX;
    GLfloat offsetY;
    const GLfloat MOVE_SPEED = 0.05f;
    
    // Uniform location
    GLint uLocation_offset;
    
public:
    HouseScene() : offsetX(0.0f), offsetY(0.0f) {}
    
    void Initialize() override {
        // Create shader program
        idProgram = glCreateProgram();
        glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, "vertex3.glsl"));
        glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment3.glsl"));
        LinkAndValidateProgram(idProgram);
        
        // Get uniform location
        glUseProgram(idProgram);
        uLocation_offset = glGetUniformLocation(idProgram, "uOffset");
        glUseProgram(0);
        
        // Configure VAO/VBO
        glGenVertexArrays(1, &idVAO);
        glBindVertexArray(idVAO);
        
        // House coordinates buffer (attribute 0)
        glGenBuffers(1, &idVBO_coord);
        glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh_Vertices), Mesh_Vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(0);
        
        printf("Scene initialized: moving house from Blender.\n");
    }
    
    void Display() override {
        glUseProgram(idProgram);
        
        // Update uniform with current offset
        glUniform2f(uLocation_offset, offsetX, offsetY);
        
        glBindVertexArray(idVAO);
        glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);
        glBindVertexArray(0);
        glUseProgram(0);
    }
    
    bool HandleKeyboard(unsigned char key, int x, int y) override {
        bool handled = false;
        
        switch (key) {
            case 'w': case 'W':
                offsetX -= MOVE_SPEED;
                handled = true;
                break;
            case 's': case 'S':
                offsetX += MOVE_SPEED;
                handled = true;
                break;
            case 'a': case 'A':
                offsetY -= MOVE_SPEED;
                handled = true;
                break;
            case 'd': case 'D':
                offsetY += MOVE_SPEED;
                handled = true;
                break;
        }
        
        return handled;
    }
    
    void Cleanup() override {
        glDeleteProgram(idProgram);
        glDeleteBuffers(1, &idVBO_coord);
        glDeleteVertexArrays(1, &idVAO);
    }
    
    const char* GetName() const override {
        return "House Scene";
    }
};

#endif
