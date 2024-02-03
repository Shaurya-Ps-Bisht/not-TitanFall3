#pragma once
#ifndef __GAME_H__
#define __GAME_H__
#include <vector>
#include <string>
#include <memory>


#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

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

#include <irrklang/irrKlang.h>
using namespace irrklang;

//#include "Player.h"

//struct DirLight {
//	glm::vec3 direction;
//	glm::vec4 color;
//};


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
	void processInput(GLFWwindow* window);
	/*unsigned int loadCubemap(std::vector<std::string> faces);
	unsigned int loadCubeMapSingle(const std::string& filePath);*/
	void addLightPoint(glm::vec3 direction, glm::vec3 color, float c, float l, float q);
	void stateCheck();

	void initDirDepth();

public:
	ISoundEngine* SoundEngine;
	bool godMode;
	lightDir m_dirLight;
	std::vector<lightPoint> m_pointLights;
	glm::mat4 lightSpaceMatrix;



private:
	int level = 1;

	unsigned int depthMapFBO;
	unsigned int depthMap;

	bool playerRuning = false;
	ISound* walkingSound = nullptr;
	ISound* grassSound = nullptr;
	ISound* seaSound = nullptr;
	
	float m_deltaTime;
	const unsigned int SCR_WIDTH = 1280;
	const unsigned int SCR_HEIGHT = 720;
	const unsigned int SHADOW_WIDTH = 2024, SHADOW_HEIGHT = 2024;


	unsigned int m_skyTexture;
	Terrain* m_terrain;
	std::vector<std::unique_ptr<Entity>> m_entities;
	std::vector<std::unique_ptr<Entity>> m_entitiesInstanced;
	SkyBox m_skyBox;
	Shader m_skyShader;
	Shader simpleDepthShader;
	Shader debugDepthQuad;

};

#endif