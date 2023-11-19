#pragma once
#ifndef __ENTITY__
#define __ENTITY__

#include "Animator.h"
#include "Camera.h"
#include "Terrain.h"
#include "lightDir.h"
#include "lightPoint.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class Entity {

protected:
public:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    Shader m_shader;
    Entity(glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader)
        : m_position(initialPosition), m_scale(initialScale), m_shader(initialShader) {}

    //virtual ~GameEntity() {};
    // Virtual method to be implemented by child classes
    virtual void draw(float deltaTime, Camera& cam, bool instanced, float elapsedTime, lightDir dLight, std::vector<lightPoint>& lightPoints, glm::mat4 lightSpaceMatrix) = 0;
    virtual void drawDirLight(float deltaTime, Camera& cam, float elapsedTime, lightDir dLight, Shader& shader) = 0;
    virtual void nice() = 0;

    // Methods to change position and scale
    void changePosition(const glm::vec3& newPosition) { m_position = newPosition; }
    void changeScale(const glm::vec3& newScale) { m_scale = newScale; }

};

// Child class with Model and Animator


// Child class without Model but with VertexBuffer
//class EntityV : public GameEntity {
//public:
//    //VertexBuffer vertexBuffer;
//
//    EntityV(glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader)
//        : GameEntity(initialPosition, initialScale, initialShader/*, vertexBuffer(initialize VertexBuffer*/) {}
//
//    void draw(float deltaTime) override {
//        // Drawing code for the object using vertex buffer...
//    }
//};


#endif