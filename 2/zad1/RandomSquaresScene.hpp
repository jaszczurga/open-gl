// -------------------------------------------------
// Scene 1: Random Colored Squares
// -------------------------------------------------
#ifndef __RANDOM_SQUARES_SCENE_HPP
#define __RANDOM_SQUARES_SCENE_HPP

#include "Scene.hpp"
#include "utilities.hpp"
#include <cstdlib>
#include <ctime>
#include <cstdio>

class RandomSquaresScene : public Scene {
private:
    static const int N = 100; // Number of squares
    static const int NUM_VERTICES_PER_SQUARE = 6; // Two triangles
    static const int TOTAL_VERTICES = N * NUM_VERTICES_PER_SQUARE;
    
    GLuint idProgram;
    GLuint idVAO;
    GLuint idVBO_coord;
    GLuint idVBO_color;
    
    // Helper functions
    float randomFloat(float min, float max) {
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
    }
    
    void GenerateData(GLfloat** coords, GLfloat** colors) {
        *coords = new GLfloat[TOTAL_VERTICES * 2]; // x, y
        *colors = new GLfloat[TOTAL_VERTICES * 3]; // r, g, b
        
        srand(time(0));
        
        for (int i = 0; i < N; ++i) {
            // Random position and size
            float cx = randomFloat(-0.9f, 0.9f);
            float cy = randomFloat(-0.9f, 0.9f);
            float size = randomFloat(0.05f, 0.15f);
            float hSize = size / 2.0f;
            
            // Random color for the entire square
            float r = randomFloat(0.1f, 1.0f);
            float g = randomFloat(0.1f, 1.0f);
            float b = randomFloat(0.1f, 1.0f);
            
            int baseCoordIndex = i * 12;
            int baseColorIndex = i * 18;
            
            // Triangle 1 (W0, W1, W3)
            (*coords)[baseCoordIndex + 0] = cx - hSize; (*coords)[baseCoordIndex + 1] = cy - hSize;
            (*coords)[baseCoordIndex + 2] = cx + hSize; (*coords)[baseCoordIndex + 3] = cy - hSize;
            (*coords)[baseCoordIndex + 4] = cx - hSize; (*coords)[baseCoordIndex + 5] = cy + hSize;
            
            // Triangle 2 (W1, W2, W3)
            (*coords)[baseCoordIndex + 6] = cx + hSize; (*coords)[baseCoordIndex + 7] = cy - hSize;
            (*coords)[baseCoordIndex + 8] = cx + hSize; (*coords)[baseCoordIndex + 9] = cy + hSize;
            (*coords)[baseCoordIndex + 10] = cx - hSize; (*coords)[baseCoordIndex + 11] = cy + hSize;
            
            // Set same color for all 6 vertices
            for(int j = 0; j < 6; ++j) {
                int colorIndex = baseColorIndex + j * 3;
                (*colors)[colorIndex + 0] = r;
                (*colors)[colorIndex + 1] = g;
                (*colors)[colorIndex + 2] = b;
            }
        }
    }
    
public:
    void Initialize() override {
        GLfloat *coords, *colors;
        GenerateData(&coords, &colors);
        
        // Create shader program
        idProgram = glCreateProgram();
        glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
        glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment1.glsl"));
        LinkAndValidateProgram(idProgram);
        
        // Configure VAO/VBO
        glGenVertexArrays(1, &idVAO);
        glBindVertexArray(idVAO);
        
        // Coordinates buffer (attribute 0)
        glGenBuffers(1, &idVBO_coord);
        glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * TOTAL_VERTICES * 2, coords, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        
        // Colors buffer (attribute 1)
        glGenBuffers(1, &idVBO_color);
        glBindBuffer(GL_ARRAY_BUFFER, idVBO_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * TOTAL_VERTICES * 3, colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
        
        delete[] coords;
        delete[] colors;
        
        printf("Scene initialized: %d random colored squares.\n", N);
    }
    
    void Display() override {
        glUseProgram(idProgram);
        glBindVertexArray(idVAO);
        glDrawArrays(GL_TRIANGLES, 0, TOTAL_VERTICES);
        glBindVertexArray(0);
        glUseProgram(0);
    }
    
    bool HandleKeyboard(unsigned char key, int x, int y) override {
        // This scene doesn't handle any specific keys
        return false;
    }
    
    void Cleanup() override {
        glDeleteProgram(idProgram);
        glDeleteBuffers(1, &idVBO_coord);
        glDeleteBuffers(1, &idVBO_color);
        glDeleteVertexArrays(1, &idVAO);
    }
    
    const char* GetName() const override {
        return "Random Squares Scene";
    }
    
    // Expose coordinate buffer for reuse by GradientSquaresScene
    GLuint GetCoordBuffer() const { return idVBO_coord; }
};

#endif
