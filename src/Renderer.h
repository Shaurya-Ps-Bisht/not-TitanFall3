#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"

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
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.45f, 0.45f, 0.45f);

	bool cursorEnabled = false;


private:
	int m_screenWidth, m_screenHeight;
	Camera* m_camera = nullptr;
	float m_lastX, m_lastY;
	bool m_firstMouse = true;


private:
	Renderer() {};
	void SetWindowProp(int width, int height);
	int InitGlfwOGL();

	void InitMesh();

};

#endif