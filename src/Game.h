#pragma once
#ifndef __GAME_H__
#define __GAME_H__

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Camera.h"
#include <vector>
//#include "Player.h"

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
	
	void InitMeshes();
	void processInput(GLFWwindow* window);
	unsigned int loadCubemap(std::vector<std::string> faces);
	unsigned int loadCubeMapSingle(const std::string& filePath);

private:
	float m_deltaTime;
	Terrain* t;

};

#endif