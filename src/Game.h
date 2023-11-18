#pragma once
#ifndef __GAME_H__
#define __GAME_H__

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Camera.h"
#include <vector>

#include "lightDir.h"
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

public:
	bool godMode;
	lightDir m_dirLight;

private:
	void GameLoop();
	void RenderLoop();
	
	void InitMeshes();
	void processInput(GLFWwindow* window);
	void setupSkybox();
	unsigned int loadCubemap(std::vector<std::string> faces);
	unsigned int loadCubeMapSingle(const std::string& filePath);

private:
	float m_deltaTime;
	const unsigned int SCR_WIDTH = 1440;
	const unsigned int SCR_HEIGHT = 900;
	Shader m_skyShader;
	unsigned int m_skyTexture;
	Terrain* m_terrain;
};

#endif