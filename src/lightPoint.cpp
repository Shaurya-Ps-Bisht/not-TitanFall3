#include "lightPoint.h"

lightPoint::lightPoint()
{
}

void lightPoint::setPointLight(glm::vec3 position, glm::vec3 color, float c, float l, float q)
{
	m_pos = position;
	m_color = color;
	constant = c;
	linear = l;
	quadratic = q;
}

void lightPoint::pointMatrixPush(std::vector<glm::mat4>& shadowTransforms)
{
	float aspect = (float)POINT_SHADOW_MAP_W / (float)POINT_SHADOW_MAP_H;
	float nearPlane = 1.0f;
	float farPlane = 25.0f;

	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);

	shadowTransforms.push_back(shadowProj *		glm::lookAt(m_pos, m_pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *		glm::lookAt(m_pos, m_pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *		glm::lookAt(m_pos, m_pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj *		glm::lookAt(m_pos, m_pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj *		glm::lookAt(m_pos, m_pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *		glm::lookAt(m_pos, m_pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

}
