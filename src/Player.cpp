#include "Player.h"


Player::~Player()
{
}


void Player::UpdatePlayerPos(glm::vec3 &newPos)
{
        m_playerModel->updatePosition(newPos);
        m_playerPos = newPos;
}

void Player::UpdatePlayerRotation(float x, float y) 
{
    //glm::vec3 front;
    //front.x = cos(glm::radians(x)) * cos(glm::radians(y));
    //front.y = sin(glm::radians(y));
    //front.z = sin(glm::radians(x)) * cos(glm::radians(y));

    m_playerModel->updateRotation(glm::vec3(90.0f - x, 0.0f, 0.0f));
    m_horizontalRotation = x;
}

void Player::InitPlayer(Camera &cam)
{
    m_playerShader = Shader("res/Shaders/skeletal.vs", "res/Shaders/skeletalPBR.fs");
    m_playerPos = glm::vec3(cam.m_cameraPos.x - 0.2f * cos(glm::radians(m_horizontalRotation)), cam.m_cameraPos.y - 1.7f,
                  cam.m_cameraPos.z - 0.2f * sin(glm::radians(m_horizontalRotation)));

    /*m_playerModel = std::make_unique<EntityM>(
        "Player",
        glm::vec3(cam.m_cameraPos.x - 0.2f * cos(glm::radians(m_horizontalRotation)), cam.m_cameraPos.y - 1.7f,
                  cam.m_cameraPos.z - 0.2f * sin(glm::radians(m_horizontalRotation)))
        , glm::vec3(.01f, .01f, .01f), m_playerShader,
                                              "res/Models/Player/Final/Player.gltf", "Idle");*/
    SetPlayerModel();
    //SetPlayerAnimations();
}

void Player::SetPlayerAnimations()
{
    //m_animations = Animation(("res/Models/Player/Final/Player.gltf"), &m_playerModel, "Idle"); //UPDATE THIS SOON
    //m_animator = Animator(&m_animations);

}

void Player::SetPlayerModel()
{
    glm::vec3 scadlee = glm::vec3(0.01f, 0.01f, 0.01f);
    m_playerModel = std::make_shared<EntityM>("Player", m_playerPos, scadlee, m_playerShader,
                                              "res/Models/Player/Final/Player.gltf",
                              "Idle");
    m_playerModel->updateRotation(glm::vec3(180.0f, 0.0f, 0.0f));
    ;

}


