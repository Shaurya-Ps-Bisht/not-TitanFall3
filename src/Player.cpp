#include "Player.h"


Player::~Player()
{
}

void Player::Draw(float dt, Camera& cam)
{
    
    m_animator.UpdateAnimation(dt);

    glm::mat4 projection = cam.GetProjectionMatrix();
    glm::mat4 view = cam.GetViewMatrix();

    {
        m_playerShader.use();
        m_playerShader.setMat4("projection", projection);
        m_playerShader.setMat4("view", view);

        auto transforms = m_animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
            m_playerShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(cam.m_cameraPos.x - 0.2f*cos(glm::radians(m_horizontalRotation)),
            cam.m_cameraPos.y - 1.7f, cam.m_cameraPos.z - 0.2f * sin(glm::radians(m_horizontalRotation)))); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(90.0f - m_horizontalRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(.01f, .01f, .01f));	// it's a bit too big for our scene, so scale it down


        m_playerShader.setMat4("model", model);
        m_playerModel.Draw(m_playerShader);
    }


}

void Player::UpdatePlayerPos(glm::vec3 posOffset)
{
    /*m_pos += posOffset;*/
}

void Player::UpdatePlayerRotation(float x, float y)
{
    glm::vec3 front;
    front.x = cos(glm::radians(x)) * cos(glm::radians(y));
    front.y = sin(glm::radians(y));
    front.z = sin(glm::radians(x)) * cos(glm::radians(y));

    m_horizontalRotation = x;
}

void Player::InitPlayer()
{
    SetPlayerModel();
    SetPlayerAnimations();
}

void Player::SetPlayerAnimations()
{
    m_animations = Animation(("res/Models/Player/Final/player.gltf"), &m_playerModel, "Idle");
    m_animator = Animator(&m_animations);

}

void Player::SetPlayerModel()
{
    m_playerShader = Shader("res/Shaders/skeletal.vs", "res/Shaders/skeletal.fs");
    m_playerModel = Model("res/Models/Player/Final/player.gltf");

}


