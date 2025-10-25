// -------------------------------------------------
// Scene 2: Gradient Colored Squares
// -------------------------------------------------
#ifndef __GRADIENT_SQUARES_SCENE_HPP
#define __GRADIENT_SQUARES_SCENE_HPP

#include "Scene.hpp"
#include "utilities.hpp"
#include <cstdlib>
#include <ctime>
#include <cstdio>

class GradientSquaresScene : public Scene {
private:
    static const int N = 100; 
    static const int NUM_VERTICES_PER_SQUARE = 6; // Two triangles
    static const int TOTAL_VERTICES = N * NUM_VERTICES_PER_SQUARE;
    
    GLuint idProgram;
    GLuint idVAO;
    GLuint idVBO_coord_shared; // Shared from Scene 1
    GLuint idVBO_color;
    
    // Helper functions
    float randomFloat(float min, float max) {
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
    }
    
    void GenerateGradientColorData(GLfloat** colors) {
        *colors = new GLfloat[TOTAL_VERTICES * 3]; // r, g, b
        
        srand(time(0));
        
        for (int i = 0; i < N; ++i) {
            // Generate 4 different colors for the 4 corners of the square
            float R[4], G[4], B[4];
            for (int k = 0; k < 4; ++k) {
                R[k] = randomFloat(0.2f, 1.0f);
                G[k] = randomFloat(0.2f, 1.0f);
                B[k] = randomFloat(0.2f, 1.0f);
            }
            
            int baseColorIndex = i * 18;
            
            // Triangle 1: W0, W1, W3
            // W0 (bottom-left, C0)
            (*colors)[baseColorIndex + 0] = R[0]; (*colors)[baseColorIndex + 1] = G[0]; (*colors)[baseColorIndex + 2] = B[0];
            // W1 (bottom-right, C1)
            (*colors)[baseColorIndex + 3] = R[1]; (*colors)[baseColorIndex + 4] = G[1]; (*colors)[baseColorIndex + 5] = B[1];
            // W3 (top-left, C3)
            (*colors)[baseColorIndex + 6] = R[3]; (*colors)[baseColorIndex + 7] = G[3]; (*colors)[baseColorIndex + 8] = B[3];
            
            // Triangle 2: W1, W2, W3
            // W1 (bottom-right, C1)
            (*colors)[baseColorIndex + 9] = R[1]; (*colors)[baseColorIndex + 10] = G[1]; (*colors)[baseColorIndex + 11] = B[1];
            // W2 (top-right, C2)
            (*colors)[baseColorIndex + 12] = R[2]; (*colors)[baseColorIndex + 13] = G[2]; (*colors)[baseColorIndex + 14] = B[2];
            // W3 (top-left, C3)
            (*colors)[baseColorIndex + 15] = R[3]; (*colors)[baseColorIndex + 16] = G[3]; (*colors)[baseColorIndex + 17] = B[3];
        }
    }
    
public:
    void Initialize() override {
        Initialize(0); // Default: no shared buffer
    }
    
    // Overload to allow sharing coordinate buffer
    void Initialize(GLuint sharedCoordBuffer) {
        GLfloat *colors;
        GenerateGradientColorData(&colors);
        
        idVBO_coord_shared = sharedCoordBuffer;
        
        // Create shader program
        idProgram = glCreateProgram();
        glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
        glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment2.glsl"));
        LinkAndValidateProgram(idProgram);
        
        // Configure VAO/VBO
        glGenVertexArrays(1, &idVAO);
        glBindVertexArray(idVAO);
        
        // Reuse coordinates buffer from Scene 1 (attribute 0)
        if (idVBO_coord_shared != 0) {
            glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord_shared);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(0);
        }
        
        // New colors buffer with gradients (attribute 1)
        glGenBuffers(1, &idVBO_color);
        glBindBuffer(GL_ARRAY_BUFFER, idVBO_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * TOTAL_VERTICES * 3, colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
        
        delete[] colors;
        
        printf("Scene initialized: %d gradient colored squares.\n", N);
    }
    
    void Display() override {
        glUseProgram(idProgram);
        glBindVertexArray(idVAO);
        glDrawArrays(GL_TRIANGLES, 0, TOTAL_VERTICES);
        glBindVertexArray(0);
        glUseProgram(0);
    }
    
    bool HandleKeyboard(unsigned char key, int x, int y) override {
        return false;
    }
    
    void Cleanup() override {
        glDeleteProgram(idProgram);
        glDeleteBuffers(1, &idVBO_color);
        glDeleteVertexArrays(1, &idVAO);
        //  We don't delete idVBO_coord_shared as it's owned by RandomSquaresScene
    }
    
    const char* GetName() const override {
        return "Gradient Squares Scene";
    }
};

#endif
