#pragma once

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>


class lightPoint
{
public:
	lightPoint();
	~lightPoint() {};

	void setPointLight(glm::vec3 position, glm::vec3 color, float c, float l, float q);
	void pointMatrixPush(std::vector<glm::mat4>& shadowTransforms);


	glm::vec3 m_pos;
	glm::vec3 m_color;
	float constant;
	float linear;
	float quadratic;

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;


};