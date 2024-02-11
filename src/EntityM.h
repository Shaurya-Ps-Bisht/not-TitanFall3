#pragma once
#ifndef __ENTITY_M__
#define __ENTITY_M__


#include "Entity.h"

class EntityM : public Entity {

public:
    // Constructor for objects without animation
    EntityM(glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader, const char* modelPath);
    // Constructor for objects with animation
    EntityM(glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader, const char* modelPath, const char* animationName);
    //Constructors for objects with explicit textures
    EntityM(const char* texturePath, glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader, const char* modelPath);
    //Constructors for objects with explicit textures and instanced rendering
    EntityM(const char* texturePath, glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader, const char* modelPath, glm::mat4* modelMatrices, unsigned int amount);
    //and this is the destructorrr
    ~EntityM() {};
    // Destructor to clean up optional objects



    // Draw method
    void draw(float deltaTime, Camera& cam, bool instanced, float elapsedTime, lightDir dLight, std::vector<lightPoint>& lightPoints, glm::mat4 lightSpaceMatrix);
    void drawDirLight(float deltaTime, bool instanced, Camera& cam, float elapsedTime, lightDir dLight, Shader& shader);
    void nice() {
        std::cout << "sad";
    }
    Model m_model;

private:
    Animation m_animation;
    Animator m_animator; 

    bool hasAnimation = false;
    bool hasExplitcitTexture = false;
    Texture explicitTexture;
    Camera m_camera;
    glm::mat4 projection = m_camera.GetProjectionMatrix();
    glm::mat4 view = m_camera.GetViewMatrix();
};

#endif