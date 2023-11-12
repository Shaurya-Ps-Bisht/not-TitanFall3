#include "Game.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"
#include "Renderer.h"
#include "Texture.h"
#include "Animation.h"
#include "Animator.h"
#include "Terrain.h"


Game::Game()
{
    m_camera = Camera(glm::vec3(0.0f, .17f, 0.0f));
    Renderer::GetInstance().setCamera(&m_camera);
}

Game::~Game()
{
}

void Game::Run()
{
    GameLoop();
}

void Game::GameLoop()
{
    float lastFrame = 0.0;
    //Shader ourShader("res/Shaders/3.3.shader.vs", "res/Shaders/3.3.shader.fs");
    Shader ourShader("res/Shaders/skeletal.vs", "res/Shaders/skeletal.fs");
    Shader skyboxShader("res/Shaders/Skybox/skybox.vs", "res/Shaders/Skybox/skybox.fs");

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    std::vector<std::string> faces
    {
        "res/CubeMaps/skybox/right.jpg",
         "res/CubeMaps/skybox/left.jpg",
         "res/CubeMaps/skybox/top.jpg",
         "res/CubeMaps/skybox/bottom.jpg",
            "res/CubeMaps/skybox/front.jpg",
            "res/CubeMaps/skybox/back.jpg"
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //unsigned int cubemapTexture = loadCubeMapSingle("res/CubeMaps/Fantasy/Cubemaps/Classic/FS000_Day_01.png");
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    //stbi_set_flip_vertically_on_load(false);
    //Model ourModel("res/Models/Backpack/backpack.obj");
    //Model ourModel("res/Models/LOTR_troll/scene.gltf");
    //Model ourModel("res/Models/LOTR_troll/fbx/source/nice.fbx");

    //Model ourModel("res/Models/Player/Vampire/dancing_vampire.dae");
    //Model ourModel("res/Models/BathRoom/dae/bathroom.dae");
    //Model ourModel("res/Models/BathRoom/dae/bathroom.dae");
    //Model ourModel("res/Models/BathRoom/gltf/scene.gltf");

    //Model ourModel("res/Models/Player/Terrorist/dae/nice.dae");
    //Model ourModel("res/Models/Player/Soldier/1/dae/nice.dae");
    Model ourModel("res/Models/Player/Final/player.gltf");

    //Model ourModel("res/Models/Player/Vampire/vampire.gltf");
    //Animation danceAnimation(("res/Models/Player/Terrorist/dae/nice.dae"), &ourModel);
    //Animation danceAnimation(("res/Models/Player/Soldier/1/dae/nice.dae"), &ourModel)
    //Animation danceAnimation(("res/Models/LOTR_troll/scene.gltf"), &ourModel, "Idle");
    Animation danceAnimation(("res/Models/Player/Final/player.gltf"), &ourModel, "Idle");
    Animator animator(&danceAnimation);

    Terrain chaljao = Terrain("res/Terrains/HeightMaps/why.png");

    
    const unsigned int SCR_WIDTH = 1440;
    const unsigned int SCR_HEIGHT = 900;

    while (!glfwWindowShouldClose(m_window))
    {
        ImGui_ImplGlfwGL3_NewFrame();

        
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(m_window);
        animator.UpdateAnimation(m_deltaTime);
     

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(m_camera.m_FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.001f, 100.0f);
        glm::mat4 view = m_camera.GetViewMatrix();
        {
            ourShader.use();

            //// view/projection transformations
            
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);

            auto transforms = animator.GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i)
                ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);


            // render the loaded model
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
            model = glm::scale(model, glm::vec3(.001f, .001, .001));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model);
            //ourModel.Draw(ourShader);
        }
        {
            chaljao.Draw(m_camera);
        }

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(m_camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        {
            static float f = 0.0f;
            static int counter = 0;
            ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
            ImGui::ColorEdit3("clear color", (float*)&(Renderer::GetInstance().clear_color)); // Edit 3 floats representing a color
            ImGui::Checkbox("Demo Window", &(Renderer::GetInstance().show_demo_window));      // Edit bools storing our windows open/close state
            ImGui::Checkbox("Another Window", &(Renderer::GetInstance().show_another_window));

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Coordinates: %.3f %.3f %.3f", m_camera.m_cameraPos.x, m_camera.m_cameraPos.y, m_camera.m_cameraPos.z);
        }
        {
            static float inputNumber = 0.5f; // Default value

            if (ImGui::InputFloat("Movement Speed", &inputNumber, 1.0f, 10.0f)) {
                m_camera.setCameraSpeed(inputNumber);
            }
        }

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Game::processInput(GLFWwindow* window)
{   if(!Renderer::GetInstance().cursorEnabled)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_camera.ProcessKeyboard(FORWARD, m_deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_camera.ProcessKeyboard(BACKWARD, m_deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_camera.ProcessKeyboard(LEFT, m_deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_camera.ProcessKeyboard(RIGHT, m_deltaTime);
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !(Renderer::GetInstance().cursorEnabled)) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            Renderer::GetInstance().cursorEnabled = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && Renderer::GetInstance().cursorEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Renderer::GetInstance().cursorEnabled = false;
    }
}

unsigned int Game::loadCubeMapSingle(const std::string& filePath)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (!data)
    {
        std::cout << "Cubemap texture failed to load at path: " << filePath << std::endl;
        stbi_image_free(data);
        return 0; // Return 0 to indicate failure
    }

    // Assuming the image has 3 rows and 4 columns
    int faceWidth = width / 4;
    int faceHeight = height / 3;

    // Set the unpack parameters
    glPixelStorei(GL_UNPACK_ROW_LENGTH, width);

    // Load each face using glTexSubImage2D
    for (int row = 0; row < 3; row++)
    {
        
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, width);

        // Calculate the offset for each face in the image
        //int xOffset = col * faceWidth;
        int yOffset = row * faceHeight;

        // Calculate the target face in the GL_TEXTURE_CUBE_MAP
        //int targetFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X + (row * 2 + (col > 1 ? 1 : 0));

        //// Set the texture image for the current face using glTexSubImage2D
        //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }

    stbi_image_free(data);

    // Reset unpack parameters to default
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;

}

unsigned int Game::loadCubemap(vector<std::string> faces)
{
    stbi_set_flip_vertically_on_load(false);

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    stbi_set_flip_vertically_on_load(true);

    return textureID;
}


