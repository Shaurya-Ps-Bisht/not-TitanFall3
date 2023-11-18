#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class lightDir
{
public:
	lightDir() {};
	~lightDir() {};

	void setDirLight(glm::vec3 direction, glm::vec3 color)
	{
		m_direction = direction;
		m_color = color;
	};


	glm::vec3 m_direction;
	glm::vec3 m_color;
};
