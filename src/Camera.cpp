#include "Camera.h"
#include <iostream>



Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    :m_lookVec(glm::vec3(0.0f, 0.0f, -1.0f)), m_movementSpeed(SPEED), m_mouseSens(SENS), m_FOV(FOV), m_cameraPos(position),
    m_WorldUp(up), m_Yaw(yaw), m_Pitch(pitch)
{
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    :m_lookVec(glm::vec3(0.0f, 0.0f, -1.0f)), m_movementSpeed(SPEED), m_mouseSens(SENS), m_FOV(FOV), m_cameraPos(glm::vec3(posX, posY, posZ)),
    m_WorldUp(glm::vec3(upX, upY, upZ)), m_Yaw(yaw), m_Pitch(pitch)
{
    updateCameraVectors();
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(m_cameraPos, m_cameraPos + m_lookVec, m_upVec);

}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
  
    float velocity = m_movementSpeed * deltaTime;
    if (direction == FORWARD)
        m_cameraPos += m_lookVec * velocity;
    if (direction == BACKWARD)
        m_cameraPos -= m_lookVec * velocity;
    if (direction == LEFT)
        m_cameraPos -= m_camRight * velocity;
    if (direction == RIGHT)
        m_cameraPos += m_camRight * velocity;

}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= m_mouseSens;
    yoffset *= m_mouseSens;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();

}

void Camera::ProcessMouseScroll(float yoffset)
{
    m_FOV -= (float)yoffset;
    if (m_FOV < 1.0f)
        m_FOV = 1.0f;
    if (m_FOV > 45.0f)
        m_FOV = 45.0f;

}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_lookVec = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_camRight = glm::normalize(glm::cross(m_lookVec, m_WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_upVec = glm::normalize(glm::cross(m_camRight, m_lookVec));

}

void Camera::setCameraSpeed(float speed)
{
    m_movementSpeed = speed;
}