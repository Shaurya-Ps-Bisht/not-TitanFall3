#include "Player.h"


Player::~Player()
{
}

void Player::Draw(float dt, Camera& cam, lightDir dLight, std::vector<lightPoint>& lightPoints)
{
    
    m_animator.UpdateAnimation(dt);

    glm::mat4 projection = cam.GetProjectionMatrix();
    glm::mat4 view = cam.GetViewMatrix();

    {
        m_playerShader.use();
        m_playerShader.setMat4("projection", projection);
        m_playerShader.setMat4("view", view);
        m_playerShader.setFloat("farPlane", cam.m_farPlane);
        m_playerShader.setFloat("pointShadowFar", 25.0f);

        //Lighting setup
        m_playerShader.setVec3("viewPos", cam.m_cameraPos);
        m_playerShader.setVec3("dirLight.direction", dLight.m_direction);
        m_playerShader.setVec3("dirLight.color", dLight.m_color);

        m_playerShader.setInt("cascadeCount", dLight.m_shadowCascadeLevels.size());
        for (size_t i = 0; i < dLight.m_shadowCascadeLevels.size(); ++i)
        {
            m_playerShader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", dLight.m_shadowCascadeLevels[i]);
        }


        for (int i = 0; i < lightPoints.size(); ++i)
        {
            std::string indexStr = std::to_string(i);

            m_playerShader.setVec3("pointLights[" + indexStr + "].position", lightPoints[i].m_pos);
            m_playerShader.setVec3("pointLights[" + indexStr + "].color", lightPoints[i].m_color);
            m_playerShader.setFloat("pointLights[" + indexStr + "].constant", lightPoints[i].constant);
            m_playerShader.setFloat("pointLights[" + indexStr + "].linear", lightPoints[i].linear);
            m_playerShader.setFloat("pointLights[" + indexStr + "].quadratic", lightPoints[i].quadratic);

            //std::cout << "SAD" << lightPoints[i].linear << std::endl;
        }


        auto transforms = m_animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
            m_playerShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(cam.m_cameraPos.x - 0.2f*cos(glm::radians(m_horizontalRotation)),
            cam.m_cameraPos.y - 1.7f, cam.m_cameraPos.z - 0.2f * sin(glm::radians(m_horizontalRotation)))); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(90.0f - m_horizontalRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(.01f, .01f, .01f));	// it's a bit too big for our scene, so scale it down


        m_playerShader.setMat4("model", model);

        /*m_playerShader.setInt("shadowMap", 11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, ShadowManager::GetInstance().m_dirLight.m_lightDepthMaps);

        m_playerShader.setInt("pointShadowMap", 12);
        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ShadowManager::GetInstance().m_depthCubemap);*/

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
    m_animations = Animation(("res/Models/Player/Final/Player.gltf"), &m_playerModel, "Idle"); //UPDATE THIS SOON
    m_animator = Animator(&m_animations);

}

void Player::SetPlayerModel()
{
    m_playerShader = Shader("res/Shaders/skeletal.vs", "res/Shaders/skeletalPBR.fs");
    m_playerModel = Model("res/Models/Player/Final/Player.gltf", true);

}


