#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//default cam values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENS = 0.1f;
const float FOV = 45.0f;

class Camera
{
public:
	float m_FOV;
	glm::vec3 m_cameraPos;
	glm::vec3 m_lookVec;
	glm::vec3 m_upVec;
	glm::vec3 m_camRight;
	glm::vec3 m_WorldUp;
	float m_Yaw;
	float m_Pitch;
	float m_movementSpeed;
	float m_mouseSens;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	~Camera();

	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

private:
	
	
	void updateCameraVectors();


};


#endif