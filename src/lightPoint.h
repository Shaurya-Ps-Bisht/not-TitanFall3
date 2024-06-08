#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <vector>


class lightPoint
{
public:
	lightPoint();
	~lightPoint() {};

	void setPointLight(glm::vec3 position, glm::vec3 color, float c, float l, float q);
	void pointMatrixPush(std::vector<glm::mat4>& shadowTransforms);
    void updatePointLightPos(const glm::vec3 &position)
    {
        m_pos = position;
    }


	glm::vec3 m_pos;
	glm::vec3 m_color;
	float constant;
	float linear;
	float quadratic;

	const unsigned int POINT_SHADOW_MAP_W = 512, POINT_SHADOW_MAP_H = 512;


};