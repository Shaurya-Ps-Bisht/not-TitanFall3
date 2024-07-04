#include "ShadowManager.h"
#include "RandomHelpers.h"

void ShadowManager::initShadows()
{
    // Directional Shadows init
    debugCascadeShader = Shader("../../res/Shaders/Depth/DebugCascade/debug_cascade.vs",
                                "../../res/Shaders/Depth/DebugCascade/debug_cascade.fs");

    initPointShadow();
    initDirectionalShadow();

    // Point shadow init
}

void ShadowManager::updateShadows(float deltaTime, float currentFrame, const std::vector<EntityPtr> &entities,
                                  Camera &cam)
{
    updateDirShadows(deltaTime, currentFrame, entities, cam);
    updatePointShadows(deltaTime, currentFrame, entities, cam);
}

void ShadowManager::updateDirShadows(float deltaTime, float currentFrame, const std::vector<EntityPtr> &entities,
                                     Camera &cam)
{
    const auto lightMatrices = m_dirLight.getLightSpaceMatrices(cam.m_nearPlane, cam.m_farPlane, cam.GetViewMatrix(),
                                                                cam.m_FOV, cam.m_aspectRatio);

    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    for (size_t i = 0; i < lightMatrices.size(); ++i)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    dirDepthShader.use();

    glBindFramebuffer(GL_FRAMEBUFFER, m_dirLight.m_lightFBO);
    glViewport(0, 0, m_dirLight.m_depthMapResolution, m_dirLight.m_depthMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT); // peter panning
    {

        for (auto &entity : entities)
        {
            std::visit(
                [&](const auto &ptr) {
                    if (!ptr->inFrustum || !ptr->getIsRendered())
                        return;
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D_ARRAY, m_dirLight.m_lightDepthMaps);
                    ptr->drawDirLight(deltaTime, false, cam, currentFrame, m_dirLight, dirDepthShader);
                },
                entity);
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

void ShadowManager::updatePointShadows(float deltaTime, float currentFrame, const std::vector<EntityPtr> &entities,
                                       Camera &cam)
{
    //When lights are moving, currently commented since they're stationary
    /*for (int i = 0; i < m_pointLights.size(); ++i)
    {
                  RandomHelpers::genCubeMapTransforms(1.5f, 25.0f, 1.0f, m_pointLights[i].m_pos, shadowTransforms, i);
    }*/

    glViewport(0, 0, POINT_SHADOW_MAP_W, POINT_SHADOW_MAP_H);
    glBindFramebuffer(GL_FRAMEBUFFER, pointDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    pointDepthShader.use();

    for (int i = 0; i < m_pointLights.size(); ++i)
    {
        if (POINT_SHADOW_MAP_W != m_pointLights[i].POINT_SHADOW_MAP_W)
        {
            POINT_SHADOW_MAP_W = m_pointLights[i].POINT_SHADOW_MAP_W;
            glViewport(0, 0, POINT_SHADOW_MAP_W, POINT_SHADOW_MAP_H);
        }
        for (unsigned int j = 0; j < 6; ++j)
            pointDepthShader.setMat4("shadowMatrices[" + std::to_string(j + 6 * i) + "]", shadowTransforms[j + 6 * i]);

        std::string indexStr = std::to_string(i);
        pointDepthShader.setVec3("lightPos[" + indexStr + "]", m_pointLights[i].m_pos);
    }

    pointDepthShader.setFloat("far_plane", far_plane);

    glCullFace(GL_FRONT); // peter panning
    {
        // m_terrain->DrawDepth(simpleDepthShader);
        /*for (const auto& obj : entities) {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D_ARRAY, m_depthCubemap);
            obj->drawDirLight(deltaTime, false, cam, currentFrame, m_dirLight, pointDepthShader);
        }*/

        for (auto &entity : entities)
        {
            std::visit(
                [&](const auto &ptr) {
                    if (!ptr->inFrustum || !ptr->getIsRendered())
                        return;
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D_ARRAY, m_depthCubemap);
                    ptr->drawDirLight(deltaTime, false, cam, currentFrame, m_dirLight, pointDepthShader);
                },
                entity);
        }

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
void ShadowManager::initPointShadow()
{
    pointDepthShader =
        Shader("../../res/Shaders/Depth/PointDepth/pointDepth.vs", "../../res/Shaders/Depth/PointDepth/pointDepth.fs",
               "../../res/Shaders/Depth/PointDepth/pointDepth.gs");

    glGenTextures(1, &m_depthCubemap);

    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_depthCubemap);

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, POINT_SHADOW_MAP_W, POINT_SHADOW_MAP_H,
                 6 * m_MaxPointLights, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

    glGenFramebuffers(1, &pointDepthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, pointDepthFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    shadowTransforms.resize(6 * m_pointLights.size());
    for (int i = 0; i < m_pointLights.size(); ++i)
    {
      RandomHelpers::genCubeMapTransforms(1.5f, 25.0f, 1.0f, m_pointLights[i].m_pos, shadowTransforms, i);
    }
}

void ShadowManager::initDirectionalShadow()
{
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_dirLight.setDirLight(glm::vec3(0.5f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    dirDepthShader = Shader("../../res/Shaders/Depth/CSM/dir_csm.vs", "../../res/Shaders/Depth/CSM/dir_csm.fs",
                            "../../res/Shaders/Depth/CSM/dir_csm.gs");
    m_dirLight.configureLightFBO();
}
