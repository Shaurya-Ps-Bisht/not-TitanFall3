#include "Camera.h"
#include "Player.h"

#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_lookVec(glm::vec3(0.0f, 0.0f, -1.0f)), m_movementSpeed(SPEED), m_mouseSens(SENS), m_FOV(FOV),
      m_cameraPos(position), m_WorldUp(up), m_Yaw(yaw), m_Pitch(pitch)
{
    updateCameraVectors();
    updateCameraFrustum();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : m_lookVec(glm::vec3(0.0f, 0.0f, -1.0f)), m_movementSpeed(SPEED), m_mouseSens(SENS), m_FOV(FOV),
      m_cameraPos(glm::vec3(posX, posY, posZ)), m_WorldUp(glm::vec3(upX, upY, upZ)), m_Yaw(yaw), m_Pitch(pitch)
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
const glm::mat4 &Camera::GetProjectionMatrix()
{
    return m_projection;
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime, const unsigned char *data,
                             const int &m_ResolutionWidth, const int &m_ResolutionHeight)
{

    float velocity = m_movementSpeed * deltaTime;

    if (direction == FORWARD)
    {
        if (godMode)
        {
            m_cameraPos += m_lookVec * velocity;
        }
        else
        {
            m_cameraPos += glm::vec3(m_lookVec.x, 0.0f, m_lookVec.z) * velocity;
        }
    }
    if (direction == BACKWARD)
    {
        if (godMode)
        {
            m_cameraPos -= m_lookVec * velocity;
        }
        else
        {
            m_cameraPos -= glm::vec3(m_lookVec.x, 0.0f, m_lookVec.z) * velocity;
        }
    }
    if (direction == LEFT)
    {
        m_cameraPos -= m_camRight * velocity;
    }
    if (direction == RIGHT)
    {
        m_cameraPos += m_camRight * velocity;
    }
    if (!godMode)
    {
        m_cameraPos.y =
            RandomHelpers::getHeight(m_cameraPos.x, m_cameraPos.z, data, m_ResolutionWidth, m_ResolutionHeight) + 1.9f;
    }

    Player::GetInstance().UpdatePlayerPos(m_cameraPos); // SHIFT TO GAME LOGIC UPATE
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
    Player::GetInstance().UpdatePlayerRotation(m_Yaw, m_Pitch);
    // Player::GetInstance().m_playerModel->updateRotation(glm::vec3(90.0f - m_Yaw, 0.0f, 0.0f));
    //;
}

void Camera::ProcessMouseScroll(float yoffset)
{
    m_FOV -= (float)yoffset;
    std::cout << "SCROLLING" << std::endl;
    if (m_FOV < 1.0f)
        m_FOV = 1.0f;
    if (m_FOV > 71.0f)
        m_FOV = 71.0f;
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_lookVec = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_camRight = glm::normalize(
        glm::cross(m_lookVec, m_WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you
                                           // look up or down which results in slower movement.
    m_upVec = glm::normalize(glm::cross(m_camRight, m_lookVec));
}

void Camera::setCameraSpeed(float speed)
{
    m_movementSpeed = speed;
}

void Camera::setPerspectiveCameraProj(float FOV, float aspectRatio, float nearPlane, float farPlane)
{
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    m_projection = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
}

void Camera::setCameraPos(glm::vec3 position)
{
    m_cameraPos = position;
}

void Camera::updateCameraFrustum()
{
    const float halfHSide = m_farPlane * tanf(FOV * .5f);
    const float halfVSide = halfHSide / m_aspectRatio;
    const glm::vec3 frontMultFar = m_farPlane * m_lookVec;

    m_frustum.nearFace = {m_cameraPos + m_nearPlane * m_lookVec, m_lookVec};
    m_frustum.farFace = {m_cameraPos + frontMultFar, -m_lookVec};
    m_frustum.rightFace = {m_cameraPos, glm::cross(frontMultFar - m_camRight * halfHSide, m_upVec)};
    m_frustum.leftFace = {m_cameraPos, glm::cross(m_upVec, frontMultFar + m_camRight * halfHSide)};
    m_frustum.topFace = {m_cameraPos, glm::cross(m_camRight, frontMultFar - m_upVec * halfVSide)};
    m_frustum.bottomFace = {m_cameraPos, glm::cross(frontMultFar + m_upVec * halfVSide, m_camRight)};
}
