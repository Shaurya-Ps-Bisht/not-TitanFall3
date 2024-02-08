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

	void addLightPoint(glm::vec3 pos, glm::vec3 color, float c, float l, float q);


	std::vector<glm::mat4> shadowTransforms;

	lightDir m_dirLight;
	std::vector<lightPoint> m_pointLights;

	glm::mat4 lightSpaceMatrix;
	std::vector<glm::mat4> lightMatricesCache;


private:
	Shader simpleDepthShader;
	Shader debugCascadeShader;
	unsigned int matricesUBO;

	

};

#endif