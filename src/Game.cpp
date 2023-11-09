#include "Game.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"
#include "Renderer.h"

Game::Game()
{
    m_camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
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
    Shader ourShader("res/Shaders/3.3.shader.vs", "res/Shaders/3.3.shader.fs");

    Model ourModel("res/Models/Backpack/backpack.obj");
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    while (!glfwWindowShouldClose(m_window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(m_window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(m_camera.m_FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = m_camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);


        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Game::processInput(GLFWwindow* window)
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

}
