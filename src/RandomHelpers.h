#pragma once

#ifndef __RANDOMHELPERS_H__
#define __RANDOMHELPERS_H__
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Terrain.h"

class RandomHelpers{
public:

    static glm::mat4* instanceMatrixTerrain(
        unsigned int amount = 500000,          
        float radius = 300.0,
        float offset = 75.0f,
        float startAngle = -20.0f,
        float endAngle = 20.0f,
        Terrain* terrain = nullptr)
    {
        glm::mat4* modelMatrices = new glm::mat4[amount];
        srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed

        for (unsigned int i = 0; i < amount; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);

            float angle = glm::mix(startAngle, endAngle, static_cast<float>(i) / static_cast<float>(amount - 1));
            float displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
            float x = -sin(glm::radians(angle)) * radius + displacement;

            displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
            float z = -cos(glm::radians(angle)) * radius + displacement;
            x += 490.0f;
            z += 750.0f;
            float y = terrain->getHeight(x, z) + 1.0f;


            model = glm::translate(model, glm::vec3(x, y, z));

            float scale = 0.01f;

            model = glm::scale(model, glm::vec3(scale));

            float rotAngle = static_cast<float>((rand() % 360));

            modelMatrices[i] = model;
        }

        return modelMatrices;

    }
};

#endif