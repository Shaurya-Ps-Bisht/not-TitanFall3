
#include "Game.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
             0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f,  0.5f, 0.0f, 1.0f, 0.0f,
        };

        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}



Game::Game()
{
    //SoundEngine = createIrrKlangDevice();
    //m_camera = Camera(glm::vec3(482.0f, -8.5f, 564.0f));
    m_camera = Camera(glm::vec3(127.362f, -98.5f, 1056.149f));
    m_camera.setPerspectiveCameraProj(70.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f);
    Renderer::GetInstance().setCamera(&m_camera);
    m_dirLight.setDirLight(glm::vec3(0.5f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

}

Game::~Game()
{
    //SoundEngine->drop();
}

void Game::Run()
{
    initDirDepth();
    initEntities();
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


        //m_dirLight.setDirLight(glm::vec3(0.5f, 1.0f, 1.0f), glm::vec3(0.8, 0.4, 0.2));
        if (level == 1) {
            if (
                //m_camera.m_cameraPos.x <= 116.0f
                1) {
                level = 2;
                m_camera.setCameraSpeed(200.0f);
                m_dirLight.setDirLight(glm::vec3(442.0f, -75.0f, 451.0f), glm::vec3(1.0f, 1.0f, 1.0f));
                lev1timeChange = currentFrame;
            }
        }
        else if (level == 2)
        {
            m_camera.setCameraPos(glm::vec3(422.0f, m_terrain->getHeight(422.0f, 437.0f) + 2.0f, 437.0f));
            if (currentFrame - lev1timeChange > 0.0f)
            {
                //grassSound = SoundEngine->play2D("res/Audio/Player/grass_ambient.mp3", true);
                level = 3;
            }
        }
        /*else if(level == 3)
        {
            m_dirLight.m_color.r -= 0.0005;
            m_dirLight.m_color.g -= 0.0005;
            m_dirLight.m_color.b -= 0.0005;
            if (m_camera.m_cameraPos.z >= 509.0f) {
                lev3timeChange = currentFrame;
                level = 4;

                addLightPoint(bMoonLoc, glm::vec3(10.0f, 0.08f, 0.08f), 1.0f, 0.09f, 0.128f);
            }
        }
        else if (level == 4) {
            m_dirLight.m_color.r -= 0.0005;
            m_dirLight.m_color.g -= 0.0005;
            m_dirLight.m_color.b -= 0.0005;
            if (currentFrame - lev3timeChange < 5.0f)
            {
                bMoonObject.m_position.y += (currentFrame - lev3timeChange)/20;
                for (size_t i = 3; i < m_pointLights.size(); i += 4) {
                    m_pointLights[i].m_pos.x += (currentFrame - lev3timeChange)/20;
                    m_pointLights[i].m_pos.y += (currentFrame - lev3timeChange)/20;
                    m_pointLights[i].m_pos.z += (currentFrame - lev3timeChange)/20;
                }

                bMoonObject.draw(m_deltaTime, m_camera, false, currentFrame, m_dirLight, m_pointLights, glm::mat4());

            }
            else {
                level = 5;
            }
        }*/
        /*else {
            bMoonObject.draw(m_deltaTime, m_camera, false, currentFrame, m_dirLight, m_pointLights, glm::mat4());
        }*/

        float near_plane = 100.0f, far_plane = 2000.0f;
        {
            glm::mat4 lightProjection, lightView;
            lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
            lightView = glm::lookAt(glm::vec3(-442.0f, 75.0f, -451.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;
            // render scene from light's point of view
            simpleDepthShader.use();
            simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

                //glCullFace(GL_FRONT);
                for (const auto& obj : m_entities) {
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, depthMap);

                    obj->drawDirLight(m_deltaTime, m_camera, currentFrame, m_dirLight, simpleDepthShader);
                }
                //glCullFace(GL_BACK);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // reset viewport
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        }

        if (level != 2)
        {
            RenderLoop();
        }

        debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", near_plane);
        debugDepthQuad.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderQuad();


        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Game::RenderLoop()
{    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float currentFrame = static_cast<float>(glfwGetTime());
    Player::GetInstance().Draw(m_deltaTime, m_camera, m_dirLight, m_pointLights);
    if (level == 3 || level == 4 || level == 5)
    {
        m_terrain->Draw(m_camera.GetProjectionMatrix(), m_camera.GetViewMatrix(), m_dirLight, m_camera.m_cameraPos);
        m_skyBox.draw(m_camera, m_dirLight.m_color);
    }

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    for (const auto& obj : m_entities) {
        /*glActiveTexture(GL_TEXTURE0+2);
        glBindTexture(GL_TEXTURE_2D, depthMap);*/

        obj->draw(m_deltaTime, m_camera, false, currentFrame, m_dirLight, m_pointLights, lightSpaceMatrix);
    }
    
    for (const auto& obj : m_entitiesInstanced) {
        obj->draw(m_deltaTime, m_camera, true, currentFrame, m_dirLight, m_pointLights, lightSpaceMatrix);
    }

    {

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Coordinates: %.3f %.3f %.3f", m_camera.m_cameraPos.x, m_camera.m_cameraPos.y, m_camera.m_cameraPos.z);
        ImGui::Text("Height: %.3f ", m_terrain->getHeight(m_camera.m_cameraPos.x, m_camera.m_cameraPos.z));
        if (ImGui::Button("GOD MODE"))                           
            m_camera.godMode = !m_camera.godMode;
    }
    {
        static glm::vec3 dirLightDirection = m_dirLight.m_direction;
        static glm::vec3 dirLightColor = m_dirLight.m_color;

        ImGui::Text("Directional Light Settings");

        if (ImGui::InputFloat3("Direction", glm::value_ptr(dirLightDirection))) {
            m_dirLight.m_direction = glm::normalize(dirLightDirection);
        }

        if (ImGui::ColorEdit3("Color", glm::value_ptr(dirLightColor))) {
            m_dirLight.m_color = dirLightColor;
        }
    }
     if(1)
    {
        static float _Scale = 1.42f;
        static float _StepScale = 4.0f;
        static float _Steps = 200;
        static float _MinHeight = 409.0f;
        static float _MaxHeight = 500.0f;
        static float _FadeDistance = 10.0f;
        static glm::vec4 _SunDir = glm::vec4(21.2000008, -1.20000005, 1.20000005, -1.35000002);

        ImGui::SliderFloat("_Scale", &_Scale, 0.1f, 10.0f);
        ImGui::SliderFloat("_StepScale", &_StepScale, 0.1f, 100.0f);
        ImGui::SliderFloat("_Steps", &_Steps, 1.0f, 200.0f);
        ImGui::SliderFloat("_MinHeight", &_MinHeight, 0.0f, 50.0f + 407.3f);
        ImGui::SliderFloat("_MaxHeight", &_MaxHeight, 60.0f + 407.3f, 1000.0f + 407.3f);
        ImGui::SliderFloat("_FadeDistance", &_FadeDistance, 0.0f, 10.0f);

        ImGui::SliderFloat4("_SunDir", glm::value_ptr(_SunDir), -1.0f, 1.0f);

        m_entities[7]->m_shader.use();
        m_entities[7]->m_shader.setFloat("_Scale", _Scale);
        m_entities[7]->m_shader.setFloat("_StepScale", _StepScale);
        m_entities[7]->m_shader.setFloat("_Steps", _Steps);
        m_entities[7]->m_shader.setFloat("_MinHeight", _MinHeight);
        m_entities[7]->m_shader.setFloat("_MaxHeight", _MaxHeight);
        m_entities[7]->m_shader.setFloat("_FadeDistance", _FadeDistance);
        m_entities[7]->m_shader.setVec4("_SunDir", glm::vec4(_SunDir)); // Add the missing W component
    }

    {
        static float inputNumber = 5.0f; // Default value

        if (ImGui::InputFloat("Movement Speed", &inputNumber, 1.0f, 10.0f)) {
            m_camera.setCameraSpeed(inputNumber);
        }
    }

    
}

void Game::processInput(GLFWwindow* window)
{

    if (!Renderer::GetInstance().cursorEnabled)
    {
        {
            bool isWPressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
            bool isSPressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
            bool isAPressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
            bool isDPressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

            // Check if any movement key is pressed
            //bool isMovingNow = isWPressed || isSPressed || isAPressed || isDPressed;

            //// If the player starts moving and there is no ongoing sound, play the audio once
            //if (isMovingNow && !playerRuning) {
            //    // Play your audio here using irrklang
            //    walkingSound = level == 1 ? 
            //        SoundEngine->play2D("res/Audio/Player/walk_wood.mp3", false, false, true):
            //        SoundEngine->play2D("res/Audio/Player/run_grass.mp3", false, false, true);
            //}

            //// If the player stops moving and there is an ongoing sound, stop the audio
            //if (!isMovingNow && playerRuning && walkingSound) {
            //    walkingSound->stop();
            //    walkingSound->drop();  // Release the sound
            //    walkingSound = nullptr;  // Reset the pointer
            //}

            //// Update the playerRuning flag
            //playerRuning = isMovingNow;



        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);


        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_camera.ProcessKeyboard(FORWARD, m_deltaTime, m_terrain);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            m_camera.ProcessKeyboard(BACKWARD, m_deltaTime, m_terrain);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            m_camera.ProcessKeyboard(LEFT, m_deltaTime, m_terrain);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_camera.ProcessKeyboard(RIGHT, m_deltaTime, m_terrain);
        }


        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !(Renderer::GetInstance().cursorEnabled)) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            Renderer::GetInstance().cursorEnabled = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && Renderer::GetInstance().cursorEnabled) {
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
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            Renderer::GetInstance().wireFrameEnabled = true;
        }
    }
}

void Game::addLightPoint(glm::vec3 pos, glm::vec3 color, float c, float l, float q)
{
    m_pointLights.push_back(lightPoint());
    m_pointLights.back().setPointLight(pos, color, c, l, q);
}



void Game::stateCheck()
{
    //if()
}

void Game::initDirDepth()
{
    simpleDepthShader = Shader("res/Shaders/Depth/Depth.vs", "res/Shaders/Depth/Depth.fs");
    debugDepthQuad = Shader("res/Shaders/Depth/Debug/depthDebug.vs", "res/Shaders/Depth/Debug/depthDebug.fs");
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);

}


void Game::initEntities()
{
    Shader bulbShader("res/Shaders/Bulb/Bulb.vs", "res/Shaders/Bulb/Bulb.fs");
    glm::vec3 lightScale1 = glm::vec3(0.025f, 0.025f, 0.025f);

    glm::vec3 lightPos1 = glm::vec3(131.430f, -97.5f, 1054.057f);
    addLightPoint(lightPos1, glm::vec3(1.0f, 0.95f, 0.8f), 1.0f, 0.09f, 0.064f);
    glm::vec3 lightPos2 = glm::vec3(137.385f, -97.5f, 1060.065f);
    addLightPoint(lightPos2, glm::vec3(1.0f, 0.95f, 0.8f), 1.0f, 0.09f, 0.064f);
    glm::vec3 lightPos3 = glm::vec3(120.423, -97.5f, 1065.115f);
    addLightPoint(lightPos3, glm::vec3(1.0f, 0.08f, 0.08f), 1.0f, 0.09f, 0.064f);

    


    std::unique_ptr<EntityV> lightBulb1 = std::make_unique<EntityV>(lightPos1, lightScale1, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), bulbShader, "SPHERE");
    std::unique_ptr<EntityV> lightBulb2 = std::make_unique<EntityV>(lightPos2, lightScale1, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), bulbShader, "SPHERE");
    std::unique_ptr<EntityV> lightBulb3 = std::make_unique<EntityV>(lightPos3, lightScale1, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), bulbShader, "SPHERE");

    m_skyBox = SkyBox("res/CubeMaps/skybox/");
    m_terrain =  new Terrain("res/Terrains/HeightMaps/heightmap1.png");   

    unsigned int amount = 500000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 300.0;
    float offset = 75.0f;
    float startAngle = -20.0f;
    float endAngle = 20.0f;

    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);

        float angle = glm::mix(startAngle, endAngle, static_cast<float>(i) / static_cast<float>(amount - 1));
        float displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float x = -sin(glm::radians(angle)) * radius + displacement;

        displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float z = -cos(glm::radians(angle)) * radius + displacement;
        x += 490.0f;
        z += 750.0f;
        float y = m_terrain->getHeight(x, z) + 1.0f;


        model = glm::translate(model, glm::vec3(x, y, z));

        float scale = 0.01f;

        model = glm::scale(model, glm::vec3(scale));

        float rotAngle = static_cast<float>((rand() % 360));

        modelMatrices[i] = model;
    }


    Shader unlitShader("res/Shaders/Standard/Unlit/Unlit.vs", "res/Shaders/Standard/Unlit/Unlit.fs");
    Shader grassShader("res/Shaders/Grass/Grass.vs", "res/Shaders/Grass/Grass.fs");
    Shader ourShader("res/Shaders/skeletal.vs", "res/Shaders/skeletal.fs");
    Shader seaShader("res/Shaders/Sea/sea.vs", "res/Shaders/Sea/sea.fs");
    Shader bMoon("res/Shaders/Bulb/bMoon/bMoon.vs", "res/Shaders/Bulb/bMoon/bMoon.fs");
    Shader vFog("res/Shaders/VolumetricFog/VolumetricFog.vs", "res/Shaders/VolumetricFog/VolumetricFog.fs");

    std::cout << "DASDSA "<< vFog.m_ID <<std::endl;

    unlitShader.use();
    unlitShader.setInt("texture_diffuse1", 0);
    unlitShader.setInt("shadowMap", 2);

    vFog.use();
    vFog.setFloat("_Scale", 2.41);
    vFog.setFloat("_StepScale", 4.3);
    vFog.setFloat("_Steps", 200);
    vFog.setFloat("_MinHeight", 0);
    vFog.setFloat("_MaxHeight", 10);
    vFog.setFloat("_FadeDistance", 10);
    vFog.setVec4("_SunDir", glm::vec4(0.2000008, 0.20000005, 1.20000005, 1));


    glm::vec3 bMoonLoc = (glm::vec3(407.3f, m_terrain->getHeight(407.3f, 365.6f) + 20.0f, 365.6f));
    glm::vec3 bMoonScale = glm::vec3(3.0f, 3.0f, 3.0f);

    glm::vec3 vFogLoc = (glm::vec3(407.3f, 407.3f, 365.6f));
    glm::vec3 vFogScale = glm::vec3(2000.0f, 100.0f, 2000.0f);
    //EntityV bMoonObject = EntityV(bMoonLoc, bMoonScale, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), bMoon, "SPHERE");
    

    Player::GetInstance().InitPlayer();
    glm::vec3 a = glm::vec3(500.0f, m_terrain->getHeight(500.0f, 570.0f) + 10.0f, 570.0f);
    glm::vec3 b = glm::vec3(1.01f, 1.01f, 1.01f);
    glm::vec3 ab = glm::vec3(5.01f, 5.01f, 5.01f);

    glm::vec3 d = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 c = glm::vec3(430.3f, m_terrain->getHeight(430.3f, 440.767f) + 30.0f, 440.767f);

    glm::vec3 ExLocation = glm::vec3(422.0f, m_terrain->getHeight(422.0f, 408.0f) + 10.0f, 408.0f);
    glm::vec3 ExLocation2 = glm::vec3(-442.0f, 75.0f, -451.0f);


    glm::vec3 InLocation = glm::vec3(120.0f, m_terrain->getHeight(120.0f, 1105.0f) - 1.0f, 1105.0f);
    glm::vec3 InScale = glm::vec3(.25f, .25f, .25f);
    glm::vec3 soljaLocation = glm::vec3(110.0f, m_terrain->getHeight(110.0f, 947.729f) + 5.0f, 947.729f);
    glm::vec3 soljaScale = glm::vec3(.01f, .01f, .01f);

    glm::vec3 boatLocation = glm::vec3(469.0f, m_terrain->getHeight(469.0f, 587.0f), 587.0f);
    glm::vec3 boatScale = glm::vec3(0.05f, 0.05f, 0.05f);

    glm::vec3 seaLocation = glm::vec3(503.0f, -13.0f, 655.5f);
    glm::vec3 seaScale = glm::vec3(100.0f, 10.0f, 200.0f);

    //std::unique_ptr<EntityM> solja = std::make_unique<EntityM>(lightPos1, soljaScale, ourShader, "res/Models/Player/Final/player.gltf", "Idle");
    std::unique_ptr<EntityM> grass = std::make_unique<EntityM>("res/textures/Grass/grass.png", a, b, grassShader, "res/Models/Grass/grass.fbx", modelMatrices, amount);
    //std::unique_ptr<EntityV> goodCube = std::make_unique<EntityV>(c, d, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), unlitShader, "SPHERE");
    std::unique_ptr<EntityM> Exterior = std::make_unique<EntityM>(ExLocation, b, unlitShader, "res/Models/House/Exterior/Exterior.gltf");
    std::unique_ptr<EntityM> Exterior2 = std::make_unique<EntityM>(ExLocation2, ab, unlitShader, "res/Models/House/Exterior/Exterior.gltf");
    std::unique_ptr<EntityM> Interior = std::make_unique<EntityM>(InLocation, InScale, unlitShader, "res/Models/House/Interior/Interior.gltf");
    std::unique_ptr<EntityM> Boat = std::make_unique<EntityM>(boatLocation, boatScale, unlitShader, "res/Models/Boat/boat.obj");
    std::unique_ptr<EntityM> sea = std::make_unique<EntityM>(seaLocation, seaScale, seaShader, "res/Models/Shapes/Plane.gltf");
    std::unique_ptr<EntityV> vFogObject = std::make_unique<EntityV>(vFogLoc, vFogScale, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), vFog, "CUBE");
    std::unique_ptr<EntityV> bMoonObject = std::make_unique<EntityV>(bMoonLoc, bMoonScale, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), unlitShader, "SPHERE");


    //m_entitiesInstanced.push_back(std::move(grass));
    //m_entities.push_back(std::move(solja));
    m_entities.push_back(std::move(Exterior));
    //m_entities.push_back(std::move(Exterior2));
    //m_entities.push_back(std::move(goodCube));
    m_entities.push_back(std::move(lightBulb1));
    m_entities.push_back(std::move(lightBulb2));
    m_entities.push_back(std::move(lightBulb3));
    m_entities.push_back(std::move(Interior));
    m_entities.push_back(std::move(Boat));
    m_entities.push_back(std::move(sea));
    m_entities.push_back(std::move(vFogObject));
    m_entities.push_back(std::move(bMoonObject));

}
//unsigned int Game::loadCubeMapSingle(const std::string& filePath)
//{
//    unsigned int textureID;
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//    int width, height, nrChannels;
//    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
//
//    if (!data)
//    {
//        std::cout << "Cubemap texture failed to load at path: " << filePath << std::endl;
//        stbi_image_free(data);
//        return 0; // Return 0 to indicate failure
//    }
//
//    // Assuming the image has 3 rows and 4 columns
//    int faceWidth = width / 4;
//    int faceHeight = height / 3;
//
//    // Set the unpack parameters
//    glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
//
//    // Load each face using glTexSubImage2D
//    for (int row = 0; row < 3; row++)
//    {
//
//        glPixelStorei(GL_UNPACK_SKIP_PIXELS, width);
//
//        // Calculate the offset for each face in the image
//        //int xOffset = col * faceWidth;
//        int yOffset = row * faceHeight;
//
//        // Calculate the target face in the GL_TEXTURE_CUBE_MAP
//        //int targetFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X + (row * 2 + (col > 1 ? 1 : 0));
//
//        //// Set the texture image for the current face using glTexSubImage2D
//        //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//    }
//
//    stbi_image_free(data);
//
//    // Reset unpack parameters to default
//    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
//
//    // Set texture parameters
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//    return textureID;
//
//}