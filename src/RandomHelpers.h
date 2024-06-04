#pragma once

#ifndef __RANDOMHELPERS_H__
#define __RANDOMHELPERS_H__
#include <glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>


namespace RandomHelpers
{

float getHeight(float x, float z, const unsigned char *data, const int &m_ResolutionWidth,
                const int &m_ResolutionHeight);

glm::mat4 *instanceMatrixTerrain(unsigned int amount, float radius, float offset, float startAngle, float endAngle,
                                 const unsigned char *data, const int &m_ResolutionWidth,
                                 const int &m_ResolutionHeight);
} // namespace RandomHelpers

#endif