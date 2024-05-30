#pragma once
#ifndef __GAME_H__
#define __GAME_H__
#include <vector>
#include <string>
#include <memory>
#include <variant>



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
#include "EntityTerrain.h"
#include "Player.h"
#include "EntityV.h"
#include "EntityM.h"
#include "lightDir.h"
#include "lightPoint.h"
#include "SkyBox.h"
#include "ShadowManager.h"

#include <irrklang/irrKlang.h>

using namespace irrklang;
using EntityPtr = std::variant< std::shared_ptr<EntityM>, std::unique_ptr<EntityM>, std::unique_ptr<EntityV>, std::unique_ptr<EntityTerrain>>;


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

	void initEntities();
  void initData();
	void processInput(GLFWwindow* window);
	void stateCheck();


	void drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);

public:
	bool godMode;
	

private:

	int level = 0;

	
	bool playerRuning = false;
	float m_deltaTime;
	

	unsigned int m_skyTexture;
  std::vector<std::shared_ptr<Entity>> m_entitiesShared;
	std::vector<std::unique_ptr<Entity>> m_entitiesUnique;
	std::vector<std::unique_ptr<Entity>> m_entitiesInstanced;
  std::vector<EntityPtr> m_entities;

	SkyBox m_skyBox;
	Shader m_skyShader;
	Shader debugDepthQuad;
	

	std::unique_ptr<EntityTerrain> m_terrain;

	unsigned char *data;
  int m_ResolutionWidth, m_ResolutionHeight, m_nrChannels;


	int debugLayer;

	std::vector<GLuint> visualizerVAOs;
	std::vector<GLuint> visualizerVBOs;
	std::vector<GLuint> visualizerEBOs;

};

#endif
