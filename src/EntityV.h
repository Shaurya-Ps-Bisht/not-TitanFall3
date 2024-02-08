#pragma once
#ifndef __ENTITY_V__
#define __ENTITY_V__

#include <string>
#include "Entity.h"
#include "Shape.h"

class EntityV : public Entity {

public:
    EntityV(glm::vec3& initialPosition, glm::vec3& initialScale, float angle, glm::vec3 axis, Shader& initialShader, const char* shape);
    ~EntityV() {};

    // Draw method
    void draw(float deltaTime, Camera& cam, bool instanced, float elapsedTime, lightDir dLight, std::vector<lightPoint>& lightPoints, glm::mat4 lightSpaceMatrix);
    void drawDirLight(float deltaTime, bool instanced, Camera& cam, float elapsedTime, lightDir dLight, Shader& shader);
    void nice() {
        std::cout << "sad";
    }
    std::vector<glm::vec3> vertexPos;

private:
    void getVertexData(const char* shape);

private:
    Camera  m_camera;
    Shape* m_shape;
    unsigned int m_vao, m_vbo, m_tbo, m_nbo, m_ibo;
    std::vector<glm::vec2> vertexUv;
    std::vector<glm::vec3> vertexNorms;
    std::vector<unsigned int> m_indices;

    float angle;
    glm::vec3 axis;

    int numVertices, numIndices;
};


#endif