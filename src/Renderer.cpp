#include <iostream>

#include "Renderer.h"
#include <stb_image.h>

Renderer::~Renderer()
{
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void Renderer::Run()
{
    SetWindowProp(1440, 900);
    InitGlfwOGL();
}


void Renderer::SetWindowProp(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
    m_lastX = m_screenWidth / 2.0f;
    m_lastY = m_screenHeight / 2.0f;

}

int Renderer::InitGlfwOGL()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(m_screenWidth, m_screenHeight, "Titanfall 3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.137255f, 0.137255f, 0.556863f, 1.0f);


    m_window = window;

    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    return 0;

}

void Renderer::setCamera(Camera* camera)
{
    m_camera = camera;
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Renderer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!Renderer::GetInstance().cursorEnabled)
    {
        GetInstance().m_camera->ProcessMouseScroll(static_cast<float>(yoffset));
    }
}


void Renderer::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (!Renderer::GetInstance().cursorEnabled)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (GetInstance().m_firstMouse)
        {
            GetInstance().m_lastX = xpos;
            GetInstance().m_lastY = ypos;
            GetInstance().m_firstMouse = false;
        }

        float xoffset = xpos - GetInstance().m_lastX;
        float yoffset = GetInstance().m_lastY - ypos; // reversed since y-coordinates go from bottom to top

        GetInstance().m_lastX = xpos;
        GetInstance().m_lastY = ypos;

        GetInstance().m_camera->ProcessMouseMovement(xoffset, yoffset);
    }
}


