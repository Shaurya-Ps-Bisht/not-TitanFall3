#pragma once

#define NOMINMAX

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <GLAD/glad.h>
#include <iostream>



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

	glm::mat4 getLightSpaceMatrix(const float& nearPlane, const float& farPlane,  const glm::mat4& view, float FOV, float aspectRatio);
	std::vector<glm::mat4> getLightSpaceMatrices(float nearPlane, float farPlane,  const glm::mat4& view, float FOV, float aspectRatio);
	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);

	void configureLightFBO();



	glm::vec3 m_direction;
	glm::vec3 m_color;
	std::vector<float> m_shadowCascadeLevels;


	unsigned int m_lightFBO;
	unsigned int m_lightDepthMaps;
	unsigned int m_depthMapResolution = 4096;


};
