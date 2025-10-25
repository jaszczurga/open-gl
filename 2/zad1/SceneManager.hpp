// -------------------------------------------------
// Scene Manager - Manages scene switching
// -------------------------------------------------
#ifndef __SCENE_MANAGER_HPP
#define __SCENE_MANAGER_HPP

#include "Scene.hpp"
#include <vector>
#include <cstdio>

class SceneManager {
private:
    std::vector<Scene*> scenes;
    int currentSceneIndex;
    
public:
    SceneManager() : currentSceneIndex(0) {}
    
    ~SceneManager() {
        // Clean up all scenes
        for (Scene* scene : scenes) {
            scene->Cleanup();
            delete scene;
        }
    }
    
    void AddScene(Scene* scene) {
        scenes.push_back(scene);
    }
    
    void DisplayCurrentScene() {
        if (currentSceneIndex >= 0 && currentSceneIndex < scenes.size()) {
            scenes[currentSceneIndex]->Display();
        }
    }
    
    bool HandleKeyboard(unsigned char key, int x, int y) {
        // Let current scene handle the key first
        if (currentSceneIndex >= 0 && currentSceneIndex < scenes.size()) {
            if (scenes[currentSceneIndex]->HandleKeyboard(key, x, y)) {
                return true;
            }
        }
        return false;
    }
    
    void NextScene() {
        if (!scenes.empty()) {
            currentSceneIndex = (currentSceneIndex + 1) % scenes.size();
            printf("Switched to: %s\n", scenes[currentSceneIndex]->GetName());
        }
    }
};

#endif
