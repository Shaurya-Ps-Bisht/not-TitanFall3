#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class lightPoint
{
public:
	lightPoint() {};
	~lightPoint() {};

	void setPointLight(glm::vec3 position, glm::vec3 color, float c, float l, float q)
	{
		m_pos = position;
		m_color = color;
		constant = c;
		linear = l;
		quadratic = q;
	};


	glm::vec3 m_pos;
	glm::vec3 m_color;
	float constant;
	float linear;
	float quadratic;
};