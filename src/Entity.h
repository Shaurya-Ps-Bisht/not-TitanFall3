#pragma once
#ifndef __ENTITY__
#define __ENTITY__

#include "Animator.h"
#include "Camera.h"
#include "Texture.h"
#include "lightDir.h"
#include "lightPoint.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Entity
{
 
  protected:
  public:
        AABB boundingAABB;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    Shader m_shader;
    bool inFrustum = true;
    bool dirtyTransform = false;

    Entity(const std::string &name, const glm::vec3 &initialPosition, const glm::vec3 &initialScale,
           const Shader &initialShader)
        : m_position(initialPosition), m_scale(initialScale), m_shader(initialShader), name(name), boundingAABB(glm::vec3(0,0,0),glm::vec3(0,0,0))
    {
    }

    const std::string &getName() const
    {
        return name;
    }

    bool getIsRendered() const
    {
        return toRender;
    }

    // Setter for isRendered
    void setIsRendered(bool value)
    {
        toRender = value;
    }

    // virtual ~GameEntity() {};
    //  Virtual method to be implemented by child classes
    virtual void draw(const float &deltaTime, Camera &cam, bool instanced, float elapsedTime, lightDir dLight,
                      std::vector<lightPoint> &lightPoints, glm::mat4 lightSpaceMatrix) = 0;
    virtual void drawDirLight(const float &deltaTime, bool instanced, Camera &cam, float elapsedTime, lightDir dLight,
                              Shader &shader) = 0;

    virtual void CalculateModelExtents() = 0;

    // Methods to change position and scale
    void updatePosition(const glm::vec3 &newPosition)
    {
        m_position = newPosition;
        dirtyTransform = true;
    }
    void updateRotation(const glm::vec3 &newRotation)
    {
        m_rotation = newRotation;
        dirtyTransform = true;
    }
    void updateScale(const glm::vec3 &newScale)
    {
        m_scale = newScale;
        dirtyTransform = true;
    } 

  private:
    bool toRender = true;
    std::string name;

  protected:
};

// Child class with Model and Animator

// Child class without Model but with VertexBuffer
// class EntityV : public GameEntity {
// public:
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
