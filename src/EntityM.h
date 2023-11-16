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
    //and this is the destructorrr
    ~EntityM() {};


    // Destructor to clean up optional objects

    // Draw method
    void draw(float deltaTime, Camera& cam);

private:
    Model m_model;
    Animation m_animation;
    Animator m_animator; 

    bool hasAnimation = false;
    Camera m_camera;
    glm::mat4 projection = m_camera.GetProjectionMatrix();
    glm::mat4 view = m_camera.GetViewMatrix();
};

#endif