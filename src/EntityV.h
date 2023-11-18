#pragma once
#ifndef __ENTITY_V__
#define __ENTITY_V__

#include <string>
#include "Entity.h"
#include "Shape.h"

class EntityV : public Entity {

public:
    EntityV(glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader, const char* shape);
    ~EntityV() {};

    // Draw method
    void draw(float deltaTime, Camera& cam, bool instanced, float elapsedTime, lightDir dLight);
private:
    void getVertexData(const char* shape);

private:
    Camera  m_camera;
    Shape* m_shape;
    unsigned int m_vao, m_vbo, m_tbo, m_nbo, m_ibo;
    std::vector<glm::vec3> vertexPos;
    std::vector<glm::vec2> vertexUv;
    std::vector<glm::vec3> vertexNorms;
    unsigned int* m_indices;

    int numVertices, numIndices;
};


#endif