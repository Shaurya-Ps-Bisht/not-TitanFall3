#include "ShadowManager.h"

void ShadowManager::initShadows()
{
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    //m_dirLight.setDirLight(glm::vec3(0.5f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));


    simpleDepthShader = Shader("res/Shaders/Depth/CSM/dir_csm.vs", "res/Shaders/Depth/CSM/dir_csm.fs", "res/Shaders/Depth/CSM/dir_csm.gs");
    debugCascadeShader = Shader("res/Shaders/Depth/DebugCascade/debug_cascade.vs", "res/Shaders/Depth/DebugCascade/debug_cascade.fs");

    m_dirLight.configureLightFBO();
}

void ShadowManager::updateShadows(float deltaTime, float currentFrame, const std::vector<std::unique_ptr<Entity>>& entities, Camera& cam)
{
    const auto lightMatrices = m_dirLight.getLightSpaceMatrices(cam.m_nearPlane, cam.m_farPlane, cam.GetViewMatrix(), cam.m_FOV, cam.m_aspectRatio);

    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    for (size_t i = 0; i < lightMatrices.size(); ++i)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    simpleDepthShader.use();

    glBindFramebuffer(GL_FRAMEBUFFER, m_dirLight.m_lightFBO);
    glViewport(0, 0, m_dirLight.m_depthMapResolution, m_dirLight.m_depthMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);  // peter panning
    {
        //m_terrain->DrawDepth(simpleDepthShader);
        for (const auto& obj : entities) {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D_ARRAY, m_dirLight.m_lightDepthMaps);
            obj->drawDirLight(deltaTime, false, cam, currentFrame, m_dirLight, simpleDepthShader);
        }

        /*for (const auto& obj : m_entitiesInstanced) {
            obj->drawDirLight(m_deltaTime, false, m_camera, currentFrame, m_dirLight, simpleDepthShader);
        }*/

    }

    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, Renderer::GetInstance().SCR_WIDTH, Renderer::GetInstance().SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ShadowManager::addLightPoint(glm::vec3 pos, glm::vec3 color, float c, float l, float q)
{
    m_pointLights.push_back(lightPoint());
    m_pointLights.back().setPointLight(pos, color, c, l, q);
}
