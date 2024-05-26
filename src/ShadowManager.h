#pragma once
#ifndef __SHADOWSMANAGER_H__
#define __SHADOWSMANAGER_H__

#include <vector>
#include <memory>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Entity.h"
#include "Renderer.h"
#include "lightDir.h"
#include "Camera.h"

class ShadowManager {
public:
	static ShadowManager& GetInstance()
	{
		static ShadowManager instance;
		return instance;
	}
	
	void initShadows();
	void updateShadows(float deltaTime, float currentFrame, const std::vector<std::unique_ptr<Entity>>& entities, Camera& cam);
	void updateDirShadows(float deltaTime, float currentFrame, const std::vector<std::unique_ptr<Entity>>& entities, Camera& cam);
	void updatePointShadows(float deltaTime, float currentFrame, const std::vector<std::unique_ptr<Entity>>& entities, Camera& cam);

	void addLightPoint(glm::vec3 pos, glm::vec3 color, float c, float l, float q);


public:
	unsigned int m_depthCubemap;

	std::vector<glm::mat4> shadowTransforms;

	lightDir m_dirLight;
	std::vector<lightPoint> m_pointLights;

	std::vector<glm::mat4> lightMatricesCache;
	glm::mat4 lightSpaceMatrix;


private:
	Shader dirDepthShader;
	Shader pointDepthShader;

	Shader debugCascadeShader;
	unsigned int matricesUBO;

	unsigned int pointDepthFBO;

	int m_MaxPointLights = 4;
	int m_MaxSpotLights = 0;
	float far_plane = 25.0f;
	
};

#endif