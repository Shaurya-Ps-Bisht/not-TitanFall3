#pragma once
#ifndef __GAME_H__
#define __GAME_H__
#include <vector>
#include <string>
#include <memory>


#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "RandomHelpers.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "Renderer.h"
#include "Texture.h"
#include "Animation.h"
#include "Animator.h"
#include "Terrain.h"
#include "Player.h"
#include "EntityV.h"
#include "EntityM.h"
#include "lightDir.h"
#include "lightPoint.h"
#include "SkyBox.h"
#include "ShadowManager.h"

#include <irrklang/irrKlang.h>
using namespace irrklang;


class Game
{
public:
	Game();
	~Game();



public:
	GLFWwindow* m_window;
	Camera m_camera;

	void Run();

private:
	void GameLoop();
	void RenderLoop();
	void UpdateShadowCubeMaps(float currentFrame);

	void initEntities();
	void processInput(GLFWwindow* window);
	void stateCheck();


	void drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);

public:
	bool godMode;
	

private:

	int level = 1;

	
	bool playerRuning = false;
	float m_deltaTime;
	

	unsigned int m_skyTexture;
	Terrain* m_terrain;
	std::vector<std::unique_ptr<Entity>> m_entities;
	std::vector<std::unique_ptr<Entity>> m_entitiesInstanced;
	SkyBox m_skyBox;
	Shader m_skyShader;
	Shader debugDepthQuad;
	

	float debugLayer;

	std::vector<GLuint> visualizerVAOs;
	std::vector<GLuint> visualizerVBOs;
	std::vector<GLuint> visualizerEBOs;

};

#endif