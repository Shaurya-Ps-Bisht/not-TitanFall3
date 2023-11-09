#pragma once
#ifndef __GAME_H__
#define __GAME_H__

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

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
	
	void InitMeshes();
	void processInput(GLFWwindow* window);

private:
	float m_deltaTime;

};

#endif