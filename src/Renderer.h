#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Game.h"



class Renderer
{
public:
	~Renderer();

	static Renderer& GetInstance()
	{
		static Renderer instance;
		return instance;
	}

	void Run();
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	void setCamera(Camera* camera);

public:
	GLFWwindow* m_window;


private:
	int m_screenWidth, m_screenHeight;
	Camera* m_camera = nullptr;
	float m_lastX, m_lastY;
	bool m_firstMouse = true;

private:
	Renderer() {};
	void SetWindowProp(int width, int height);
	int InitGlfwOGL();



};

#endif