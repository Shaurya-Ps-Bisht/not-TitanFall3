
#include "Game.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#ifdef __cplusplus
extern "C"
{
#endif

    __declspec(dllexport) DWORD NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

// namespace
//{
//	ShadowManager& GetShadowManagerInstance()
//	{
//		return ShadowManager::GetInstance();
//	}
// }

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            /*0.25f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.25f, 0.5f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 0.5f, 0.0f, 1.0f, 0.0f,*/
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
        };

        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

Game::Game()
{
    // SoundEngine = createIrrKlangDevice();
    // m_camera = Camera(glm::vec3(482.0f, -8.5f, 564.0f));
    m_camera = Camera(glm::vec3(128.5f, -100.8f, 1104.0f));
    m_camera.setPerspectiveCameraProj(
        70.0f, (float)Renderer::GetInstance().SCR_WIDTH / (float)Renderer::GetInstance().SCR_HEIGHT, 0.1f, 5000.0f);
    Renderer::GetInstance().setCamera(&m_camera);
}

Game::~Game()
{
    // SoundEngine->drop();
    stbi_image_free(data);
}

void Game::Run()
{
    initData();
    initEntities();
    ShadowManager::GetInstance().initShadows();
    debugDepthQuad = Shader("res/Shaders/Depth/Debug/depthDebug.vs", "res/Shaders/Depth/Debug/depthDebug.fs");
    debugDepthQuad.use();
    debugDepthQuad.setInt("shadowMap", 2);
    GameLoop();
}

void Game::GameLoop()
{
    float lastFrame = 0.0;
    float lev1timeChange = 0.0f;
    float lev3timeChange = 0.0f;

    while (!glfwWindowShouldClose(m_window))
    {

        ImGui_ImplGlfwGL3_NewFrame();

        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(m_window);

        if (level == 1)
        {
            ShadowManager::GetInstance().m_dirLight.setDirLight(glm::vec3(442.0f, -75.0f, 451.0f),
                                                                glm::vec3(1.0f, 1.0f, 1.0f));
            if (
                // m_camera.m_cameraPos.x <= 116.0f
                1)
            {
                level = 2;
                m_camera.setCameraSpeed(200.0f);

                lev1timeChange = currentFrame;
            }
        }
        else if (level == 2)
        {
            m_camera.setCameraPos(glm::vec3(
                422.0f, RandomHelpers::getHeight(422.0f, 437.0f, data, m_ResolutionWidth, m_ResolutionHeight) + 2.0f,
                437.0f));
            if (currentFrame - lev1timeChange > 0.0f)
            {
                // grassSound = SoundEngine->play2D("res/Audio/Player/grass_ambient.mp3", true);
                level = 3;
            }
        }

        if (level != 2)
        {
            ShadowManager::GetInstance().updateShadows(m_deltaTime, currentFrame, m_entities, m_camera);
            RenderLoop();
        }
        {
            /*if (lightMatricesCache.size() != 0)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                debugCascadeShader.use();
                debugCascadeShader.setMat4("projection", m_camera.m_projection);
                debugCascadeShader.setMat4("view", m_camera.GetViewMatrix());
                drawCascadeVolumeVisualizers(lightMatricesCache, &debugCascadeShader);
                glDisable(GL_BLEND);
            }*/
        }

        debugDepthQuad.use();
        debugDepthQuad.setInt("layer", debugLayer);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_ARRAY, ShadowManager::GetInstance().m_dirLight.m_lightDepthMaps);

        // renderQuad();

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Game::RenderLoop()
{

    float currentFrame = static_cast<float>(glfwGetTime());

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_skyBox.draw(m_camera, ShadowManager::GetInstance().m_dirLight.m_color);
    for (auto &entity : m_entities)
    {
        std::visit(
            [&](const auto &ptr) {
                if (!ptr->getIsRendered())
                    return;
                ptr->draw(m_deltaTime, m_camera, false, currentFrame, ShadowManager::GetInstance().m_dirLight,
                          ShadowManager::GetInstance().m_pointLights, ShadowManager::GetInstance().lightSpaceMatrix);
            },
            entity);
    }

    for (const auto &obj : m_entitiesInstanced)
    {
        obj->draw(m_deltaTime, m_camera, true, currentFrame, ShadowManager::GetInstance().m_dirLight,
                  ShadowManager::GetInstance().m_pointLights, ShadowManager::GetInstance().lightSpaceMatrix);
    }

    {
        bool horizontal = true, first_iteration = true;
        int amount = 10;
        shaderBlur.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            shaderBlur.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongBuffer[!horizontal]);
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    {

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::Text("Coordinates: %.3f %.3f %.3f", m_camera.m_cameraPos.x, m_camera.m_cameraPos.y,
                    m_camera.m_cameraPos.z);
        ImGui::Text("Height: %.3f ", RandomHelpers::getHeight(m_camera.m_cameraPos.x, m_camera.m_cameraPos.z, data,
                                                              m_ResolutionWidth, m_ResolutionHeight));
        if (ImGui::Button("GOD MODE"))
            m_camera.godMode = !m_camera.godMode;
    }

    {
        static float inputNumber = 5.0f; // Default value

        if (ImGui::InputFloat("Movement Speed", &inputNumber, 1.0f, 10.0f))
        {
            m_camera.setCameraSpeed(inputNumber);
        }
    }

    if (ImGui::CollapsingHeader("HDR"))
    {
        if (ImGui::Button("HDR Toggle"))
            hdr = !hdr;

        ImGui::SliderFloat("Exposure", &exposure, 0.0f, 10.0f);
    }

    if (ImGui::CollapsingHeader("Directional Light Settings"))
    {

        static glm::vec3 dirLightDirection = ShadowManager::GetInstance().m_dirLight.m_direction;
        static glm::vec3 dirLightColor = ShadowManager::GetInstance().m_dirLight.m_color;

        if (ImGui::InputFloat3("Direction", glm::value_ptr(dirLightDirection)))
        {
            ShadowManager::GetInstance().m_dirLight.m_direction = glm::normalize(dirLightDirection);
        }

        if (ImGui::ColorEdit3("Color", glm::value_ptr(dirLightColor)))
        {
            ShadowManager::GetInstance().m_dirLight.m_color = dirLightColor;
        }
    }

    if (ImGui::CollapsingHeader("Volumetric Fog"))
    {
        static float _Scale = 1.42f;
        static float _StepScale = 4.0f;
        static float _Steps = 200;
        static float _MinHeight = 0;
        static float _MaxHeight = 1000.0f + 407.3f;
        static float _FadeDistance = 100.0f;
        static glm::vec4 _SunDir = glm::vec4(21.2000008, -1.20000005, 1.20000005, -1.35000002);

        ImGui::SliderFloat("_Scale", &_Scale, 0.1f, 10.0f);
        ImGui::SliderFloat("_StepScale", &_StepScale, 0.1f, 100.0f);
        ImGui::SliderFloat("_Steps", &_Steps, 1.0f, 200.0f);
        ImGui::SliderFloat("_MinHeight", &_MinHeight, 0.0f, 50.0f + 407.3f);
        ImGui::SliderFloat("_MaxHeight", &_MaxHeight, 60.0f + 407.3f, 1000.0f + 407.3f);
        ImGui::SliderFloat("_FadeDistance", &_FadeDistance, 0.0f, 1000.0f);

        ImGui::SliderFloat4("_SunDir", glm::value_ptr(_SunDir), -1.0f, 1.0f);

        //    m_entities[8]->m_shader.use();
        //    m_entities[8]->m_shader.setFloat("_Scale", _Scale);
        //    m_entities[8]->m_shader.setFloat("_StepScale", _StepScale);
        //    m_entities[8]->m_shader.setFloat("_Steps", _Steps);
        //    m_entities[8]->m_shader.setFloat("_MinHeight", _MinHeight);
        //    m_entities[8]->m_shader.setFloat("_MaxHeight", _MaxHeight);
        //    m_entities[8]->m_shader.setFloat("_FadeDistance", _FadeDistance);
        //    m_entities[8]->m_shader.setVec4("_SunDir", glm::vec4(_SunDir)); // Add the missing W component
    }

    ImGui::Begin("Entities");

    for (auto &entity : m_entities)
    {
        std::visit(
            [](const auto &ptr) {
                // ptr->draw(); // Call draw() or other member functions
                bool isRendered = ptr->getIsRendered();
                if (ImGui::Checkbox(ptr->getName().c_str(), &isRendered))
                {
                    ptr->setIsRendered(isRendered);
                }
            },
            entity);
    }

    ImGui::End();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    hdrShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);

    hdrShader.setInt("hdr", hdr);
    hdrShader.setFloat("exposure", exposure);
    renderQuad();
}

void Game::initEntities()
{
    Player::GetInstance().InitPlayer(m_camera);

    m_skyBox = SkyBox("res/CubeMaps/skybox/");
    m_terrain = std::make_unique<EntityTerrain>("Terrain 1", data, m_ResolutionWidth, m_ResolutionHeight);

    //----------------------------------------------------------------------------------------------------------

    // glm::vec3 bMoonLoc = (glm::vec3(407.3f, m_terrain->getHeight(407.3f, 365.6f) + 20.0f, 365.6f));
    glm::vec3 bMoonLoc = (glm::vec3(413.222f, 24.290, 364.421f));
    glm::vec3 ExLocation = glm::vec3(
        422.0f, RandomHelpers::getHeight(422.0f, 408.0f, data, m_ResolutionWidth, m_ResolutionHeight) + 10.0f, 408.0f);
    glm::vec3 ExLocation2 = glm::vec3(-442.0f, 75.0f, -451.0f);
    glm::vec3 InLocation = glm::vec3(
        120.0f, RandomHelpers::getHeight(120.0f, 1105.0f, data, m_ResolutionWidth, m_ResolutionHeight) - 1.0f, 1105.0f);
    glm::vec3 soljaLocation1 = glm::vec3(127.387f, -100.8f, 1104.9f);
    glm::vec3 soljaLocation = glm::vec3(127.387f, -102.4f, 1104.9f);
    glm::vec3 vampireLocation = glm::vec3(102.085f, -101.8f, 1105.133f);
    glm::vec3 boatLocation = glm::vec3(
        469.0f, RandomHelpers::getHeight(469.0f, 587.0f, data, m_ResolutionWidth, m_ResolutionHeight), 587.0f);
    glm::vec3 vFogLoc = (glm::vec3(407.3f, 407.3f, 365.6f));
    glm::vec3 seaLocation = glm::vec3(503.0f, -13.0f, 655.5f);
    glm::vec3 grassPos = glm::vec3(
        500.0f, RandomHelpers::getHeight(500.0f, 570.0f, data, m_ResolutionWidth, m_ResolutionHeight) + 10.0f, 570.0f);
    glm::vec3 c = glm::vec3(
        430.3f, RandomHelpers::getHeight(430.3f, 440.767f, data, m_ResolutionWidth, m_ResolutionHeight) + 30.0f,
        440.767f);

    glm::vec3 lightPos1 = glm::vec3(128.5f, -100.8f, 1104.0f);
    glm::vec3 lightPos2 = glm::vec3(129.356f, -100.8f, 1105.99f);
    glm::vec3 lightPos3 = glm::vec3(100.95f, -100.8f, 1104.095f);
    glm::vec3 lightPos4 = glm::vec3(120000.423, -973333.5f, 1333065.115f);

    //----------------------------------------------------------------------------------------------------------
    glm::vec3 bMoonScale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 vFogScale = glm::vec3(2000.0f, 100.0f, 2000.0f);
    // EntityV bMoonObject = EntityV(bMoonLoc, bMoonScale, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), bMoon, "SPHERE");
    glm::vec3 grassScale = glm::vec3(1.01f, 1.01f, 1.01f);
    glm::vec3 InScale = glm::vec3(1.f, 1.f, 1.0f);
    glm::vec3 soljaScale1 = glm::vec3(01.1f, 01.1f, 01.1f);
    glm::vec3 soljaScale = glm::vec3(0.01f, 0.01f, 0.01f);
    glm::vec3 boatScale = glm::vec3(0.05f, 0.05f, 0.05f);
    glm::vec3 seaScale = glm::vec3(100.0f, 10.0f, 200.0f);
    glm::vec3 d = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 ab = glm::vec3(5.01f, 5.01f, 5.01f);
    glm::vec3 lightScale1 = glm::vec3(0.025f, 0.025f, 0.025f);
    glm::vec3 vampireScale = glm::vec3(1.1f, 1.1f, 1.1f);

    //----------------------------------------------------------------------------------------------------------

    Shader bulbShader("res/Shaders/Bulb/Bulb.vs", "res/Shaders/Bulb/Bulb.fs");
    Shader unlitShader("res/Shaders/Standard/Unlit/Unlit.vs", "res/Shaders/Standard/Unlit/Unlit.fs");
    Shader pbrShader("res/Shaders/Standard/PBR/PBR.vs", "res/Shaders/Standard/PBR/PBR.fs");
    Shader grassShader("res/Shaders/Grass/Grass.vs", "res/Shaders/Grass/Grass.fs");
    Shader ourShader("res/Shaders/skeletal.vs", "res/Shaders/skeletalPBR.fs");
    Shader seaShader("res/Shaders/Sea/sea.vs", "res/Shaders/Sea/sea.fs");
    Shader bMoon("res/Shaders/Bulb/bMoon/bMoon.vs", "res/Shaders/Bulb/bMoon/bMoon.fs");
    // Shader vFog("res/Shaders/VolumetricFog/VolumetricFog.vs", "res/Shaders/VolumetricFog/VolumetricFog.fs");

    unlitShader.use();
    unlitShader.setInt("shadowMap", 11);
    unlitShader.setInt("pointShadowMap", 12);

    /*pbrShader.use();
    pbrShader.setInt("shadowmap", 11);
    pbrShader.setInt("pointshadowmap", 12);

    ourShader.use();
    ourShader.setInt("shadowmap", 11);
    ourShader.setInt("pointshadowmap", 12);*/ //DELETE THIS

    /*vFog.use();
    vFog.setFloat("_Scale", 2.41);
    vFog.setFloat("_StepScale", 4.3);
    vFog.setFloat("_Steps", 200);
    vFog.setFloat("_MinHeight", 0);
    vFog.setFloat("_MaxHeight", 10);
    vFog.setFloat("_FadeDistance", 10);
    vFog.setVec4("_SunDir", glm::vec4(0.2000008, 0.20000005, 1.20000005, 1));*/

    std::unique_ptr<EntityM> grass = std::make_unique<EntityM>(
        "Grass", "res/textures/Grass/grass.png", grassPos, grassScale, grassShader, "res/Models/Grass/grass.fbx",
        RandomHelpers::instanceMatrixTerrain(500000, 300.0, 75.0f, -20.0f, 20.0f, data, m_ResolutionWidth,
                                             m_ResolutionHeight),
        500000);

    std::unique_ptr<EntityV> lightBulb1 = std::make_unique<EntityV>("bulb 1", lightPos1, lightScale1, 0.0f,
                                                                    glm::vec3(1.0f, 0.0f, 0.0f), bulbShader, "SPHERE");
    std::unique_ptr<EntityV> lightBulb2 = std::make_unique<EntityV>("bulb 2", lightPos2, lightScale1, 0.0f,
                                                                    glm::vec3(1.0f, 0.0f, 0.0f), bulbShader, "SPHERE");
    std::unique_ptr<EntityV> lightBulb3 = std::make_unique<EntityV>("bulb 3", lightPos4, lightScale1, 0.0f,
                                                                    glm::vec3(1.0f, 0.0f, 0.0f), bulbShader, "SPHERE");
    // std::unique_ptr<EntityM> solja = std::make_unique<EntityM>(soljaLocation, soljaScale, ourShader,
    // "res/Models/Player/Final/Player.gltf", "Idle"); std::unique_ptr<EntityM> vampire =
    //  std::make_unique<EntityM>(vampireLocation, vampireScale, ourShader,
    //  "res/Models/Player/Vampire/dancing_vampire.dae", "Hips");
    // vampire->m_model.
    // std::unique_ptr<EntityM> cyberGirl = std::make_unique<EntityM>(soljaLocation, soljaScale1, ourShader,
    // "res/Models/Player/Cybergirl/scene.gltf", "pose1"); std::unique_ptr<EntityM> backpack =
    // std::make_unique<EntityM>(soljaLocation1, soljaScale1, unlitShader,
    // "res/Models/Backpack/Survival_BackPack_2.fbx"); std::unique_ptr<EntityV> goodCube = std::make_unique<EntityV>(c,
    // d, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), unlitShader, "SPHERE");
    std::unique_ptr<EntityM> Exterior = std::make_unique<EntityM>("House Exterior", ExLocation, grassScale, pbrShader,
                                                                  "res/Models/House/Exterior/Exterior.gltf");
    // std::unique_ptr<EntityM> Exterior2 = std::make_unique<EntityM>(ExLocation2, ab, unlitShader,
    // "res/Models/House/Exterior/Exterior.gltf");
    std::unique_ptr<EntityM> Interior = std::make_unique<EntityM>("Interior", InLocation, InScale, pbrShader,
                                                                  "res/Models/House/StarWarsClone/untitled.gltf");
    std::unique_ptr<EntityM> Boat =
        std::make_unique<EntityM>("BOAT", boatLocation, boatScale, unlitShader, "res/Models/Boat/boat.obj");
    // std::unique_ptr<EntityV> vFogObject = std::make_unique<EntityV>(vFogLoc, vFogScale, 0.0f, glm::vec3(1.0f, 0.0f,
    // 0.0f), vFog, "CUBE");
    std::unique_ptr<EntityM> sea =
        std::make_unique<EntityM>("SEA", seaLocation, seaScale, seaShader, "res/Models/Shapes/Plane.gltf");
    std::unique_ptr<EntityV> bMoonObject = std::make_unique<EntityV>(
        "Moon", bMoonLoc, bMoonScale, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), unlitShader, "SPHERE");
    // std::unique_ptr<EntityM> solja = std::make_unique<EntityM>(soljaLocation, soljaScale, ourShader,
    // "res/Models/Player/Final/Player.gltf", "Idle"); std::unique_ptr<EntityM> vampire =

    // cyberGirl->m_model.loadTexturesInfo();

    m_entitiesInstanced.push_back(std::move(grass));

    // m_entities.push_back(std::move(backpack));
    // m_entities.push_back(std::move(vampire));
    // m_entities.push_back(std::move(solja));
    // m_entities.push_back(std::move(cyberGirl));
    // m_entities.push_back(std::move(Exterior2));
    // m_entities.push_back(std::move(goodCube));
    m_entities.emplace_back(std::move(m_terrain));
    m_entities.emplace_back(std::move(Exterior));
    m_entities.emplace_back(std::move(lightBulb1));
    m_entities.emplace_back(std::move(lightBulb2));
    m_entities.emplace_back(std::move(lightBulb3));
    m_entities.emplace_back(std::move(Interior));
    m_entities.emplace_back(std::move(Boat));
    // m_entities.emplace_back(std::move(vFogObject));
    m_entities.emplace_back(std::move(bMoonObject));
    m_entities.emplace_back(std::move(sea));

    m_entities.emplace_back(Player::GetInstance().m_playerModel);
}

void Game::initData()
{
    unsigned int SCR_WIDTH = Renderer::GetInstance().SCR_WIDTH;
    unsigned int SCR_HEIGHT = Renderer::GetInstance().SCR_HEIGHT;

    {
        glGenFramebuffers(1, &hdrFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

        glGenTextures(2, colorBuffers);
        for (unsigned int i = 0; i < 2; i++)
        {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
        }

        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, attachments);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        hdrShader = Shader("res/Shaders/Post Processing/HDR/hdr.vs", "res/Shaders/Post Processing/HDR/hdr.fs");
        hdrShader.use();
        hdrShader.setInt("hdrBuffer", 0);
        hdrShader.setInt("bloomBlur", 1);
    }
    {
        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongBuffer);
        for (unsigned int i = 0; i < 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
        }

        shaderBlur = Shader("res/Shaders/Post Processing/Bloom/blur.vs", "res/Shaders/Post Processing/Bloom/blur.fs");
    }

    int width, height, nrChannels;
    data = stbi_load("res/Terrains/HeightMaps/heightmap1.png", &width, &height, &nrChannels, 0);
    if (data)
    {

        m_ResolutionWidth = width;
        m_ResolutionHeight = height;
        m_nrChannels = nrChannels;

        std::cout << "Loaded heightmap of size " << m_ResolutionWidth << " x " << m_ResolutionHeight
                  << " CHANNELS: " << m_nrChannels << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    glm::vec3 lightPos1 = glm::vec3(128.5f, -100.8f, 1104.0f);
    glm::vec3 lightPos2 = glm::vec3(129.356f, -100.8f, 1105.99f);
    glm::vec3 lightPos3 = glm::vec3(100.95f, -100.8f, 1104.095f);
    glm::vec3 lightPos4 = glm::vec3(120000.423, -973333.5f, 1333065.115f);

    ShadowManager::GetInstance().addLightPoint(lightPos1, glm::vec3(0.0f, 1.95f, 0.8f), 1.0f, 0.09f, 0.064f);
    ShadowManager::GetInstance().addLightPoint(lightPos2, glm::vec3(1.0f, 0.95f, 0.8f), 1.0f, 0.09f, 0.064f);
    // ShadowManager::GetInstance().addLightPoint(lightPos3, glm::vec3(1000.0f, 0.08f, 0.08f), 1.0f, 0.09f, 0.064f);
    // ShadowManager::GetInstance().addLightPoint(lightPos4, glm::vec3(1000.0f, 0.08f, 0.08f), 1.0f, 0.09f, 0.064f);
}

void Game::processInput(GLFWwindow *window)
{

    if (!Renderer::GetInstance().cursorEnabled)
    {
        {
            bool isWPressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
            bool isSPressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
            bool isAPressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
            bool isDPressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_camera.ProcessKeyboard(FORWARD, m_deltaTime, data, m_ResolutionWidth, m_ResolutionHeight);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            m_camera.ProcessKeyboard(BACKWARD, m_deltaTime, data, m_ResolutionWidth, m_ResolutionHeight);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            m_camera.ProcessKeyboard(LEFT, m_deltaTime, data, m_ResolutionWidth, m_ResolutionHeight);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_camera.ProcessKeyboard(RIGHT, m_deltaTime, data, m_ResolutionWidth, m_ResolutionHeight);
        }

        {
            static int plusPress = GLFW_RELEASE;
            if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE && plusPress == GLFW_PRESS)
            {
                debugLayer++;
                if (debugLayer > ShadowManager::GetInstance().m_dirLight.m_shadowCascadeLevels.size())
                {
                    debugLayer = 0;
                }
                std::cout << debugLayer << std::endl;
            }
            plusPress = glfwGetKey(window, GLFW_KEY_N);

            static int cPress = GLFW_RELEASE;
            if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && cPress == GLFW_PRESS)
            {
                ShadowManager::GetInstance().lightMatricesCache =
                    ShadowManager::GetInstance().m_dirLight.getLightSpaceMatrices(
                        m_camera.m_nearPlane, m_camera.m_farPlane, m_camera.GetViewMatrix(), m_camera.m_FOV,
                        m_camera.m_aspectRatio);
            }
            cPress = glfwGetKey(window, GLFW_KEY_C);
        }

        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !(Renderer::GetInstance().cursorEnabled))
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            Renderer::GetInstance().cursorEnabled = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && Renderer::GetInstance().cursorEnabled)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Renderer::GetInstance().cursorEnabled = false;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        if (Renderer::GetInstance().wireFrameEnabled)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            Renderer::GetInstance().wireFrameEnabled = false;
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            Renderer::GetInstance().wireFrameEnabled = true;
        }
    }
}

void Game::drawCascadeVolumeVisualizers(const std::vector<glm::mat4> &lightMatrices, Shader *shader)
{
    visualizerVAOs.resize(8);
    visualizerEBOs.resize(8);
    visualizerVBOs.resize(8);

    const GLuint indices[] = {0, 2, 3, 0, 3, 1, 4, 6, 2, 4, 2, 0, 5, 7, 6, 5, 6, 4,
                              1, 3, 7, 1, 7, 5, 6, 7, 3, 6, 3, 2, 1, 5, 4, 0, 1, 4};

    const glm::vec4 colors[] = {
        {1.0, 0.0, 0.0, 0.5f},
        {0.0, 1.0, 0.0, 0.5f},
        {0.0, 0.0, 1.0, 0.5f},
    };

    for (int i = 0; i < lightMatrices.size(); ++i)
    {
        const auto corners = ShadowManager::GetInstance().m_dirLight.getFrustumCornersWorldSpace(lightMatrices[i]);
        std::vector<glm::vec3> vec3s;
        for (const auto &v : corners)
        {
            vec3s.push_back(glm::vec3(v));
        }

        glGenVertexArrays(1, &visualizerVAOs[i]);
        glGenBuffers(1, &visualizerVBOs[i]);
        glGenBuffers(1, &visualizerEBOs[i]);

        glBindVertexArray(visualizerVAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, visualizerVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizerEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

        glBindVertexArray(visualizerVAOs[i]);
        shader->setVec4("color", colors[i % 3]);
        glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);

        glDeleteBuffers(1, &visualizerVBOs[i]);
        glDeleteBuffers(1, &visualizerEBOs[i]);
        glDeleteVertexArrays(1, &visualizerVAOs[i]);

        glBindVertexArray(0);
    }

    visualizerVAOs.clear();
    visualizerEBOs.clear();
    visualizerVBOs.clear();
}

void Game::stateCheck()
{
    // if()
}
