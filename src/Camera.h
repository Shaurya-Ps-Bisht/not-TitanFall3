#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Terrain.h"

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
const float SPEED = 5.0f;
const float SENS = 0.1f;
const float FOV = 70.0f;

class Camera
{
public:
	float m_FOV;
	glm::vec3 m_cameraPos;
	glm::vec3 m_lookVec;
	glm::vec3 m_upVec;
	glm::vec3 m_camRight;
	glm::vec3 m_WorldUp;
	glm::mat4 m_projection;

	float m_Yaw;
	float m_Pitch;
	float m_movementSpeed;
	float m_mouseSens;
	bool godMode = true;
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	~Camera();

	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(Camera_Movement direction, float deltaTime, Terrain* t);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);
	void setCameraSpeed(float speed);
	void setPerspectiveCameraProj(float FOV, float aspectRatio, float nearPlane, float farPlane);
	void setCameraPos(glm::vec3 position);
	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);

	glm::mat4& GetProjectionMatrix() { return m_projection; }

private:
	
	int countA = 0;
	int countB = 0;
	void updateCameraVectors();

};


#endif