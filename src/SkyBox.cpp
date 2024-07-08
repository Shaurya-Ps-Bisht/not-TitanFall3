#include "SkyBox.h"
#include "RandomHelpers.h"

SkyBox::SkyBox(const char *path)
{
    skyboxShader = Shader("../../res/Shaders/Skybox/skyBox.vs", "../../res/Shaders/Skybox/skybox.fs");
    hdrSkyboxShader = Shader("../../res/Shaders/Skybox/hdrSkybox.vs", "../../res/Shaders/Skybox/hdrSkybox.fs");

    {
        // generic use frameBuffer generation and layered depth buffer
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // glGenTextures(1, &depthBuffer);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, depthBuffer);
        // for (unsigned int i = 0; i < 6; ++i)
        // {
        //     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, 512, 512, 0,
        //     GL_DEPTH_COMPONENT,
        //                  GL_FLOAT, nullptr);
        // }
        // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    std::vector<std::string> faces{"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"};

    for (std::string &face : faces)
    {
        face = path + face;
    }

    // cubeMapTex = loadCubemap(faces);
    hdrCubeMapTex = loadCubemapHDR("../../res/CubeMaps/skybox/kloppenheim_06_puresky_2k.hdr");
    equirecToCubemap(); // sets cubemaptex to this CHAAAAAAAANGE THIS LATER

    ReflectionProbe::GetInstance().generateSkyBoxIrradianceMap(cubeMapTex);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
}

void SkyBox::draw(Camera &cam, glm::vec3(color))
{

    glm::mat4 view = glm::mat4(glm::mat3(cam.GetViewMatrix())); // remove translation from the view matrix
    glm::mat4 projection = cam.GetProjectionMatrix();
    glDepthFunc(GL_LEQUAL);
    skyboxShader.use();
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    skyboxShader.setVec3("lightColor", color);
    // skybox cube
    // glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTex);

    RandomHelpers::renderCube();
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

    // equirecToCubemap();
}

unsigned int SkyBox::loadCubemap(std::vector<std::string> faces)
{
    stbi_set_flip_vertically_on_load(false);

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         data);
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

unsigned int SkyBox::loadCubemapHDR(const std::string &filePath)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(filePath.c_str(), &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
    return hdrTexture;
}
void SkyBox::equirecToCubemap()
{
    Shader equirecToCubemapShader = Shader("../../res/Shaders/Standard/Cubemaps/cubemap_capture.vs",
                                           "../../res/Shaders/Standard/Cubemaps/equiToCubemap.fs",
                                           "../../res/Shaders/Standard/Cubemaps/cubemap_capture.gs");

    // Shader equirecToCubemapShader =
    // Shader("../../res/Shaders/Standard/Cubemaps/b.vs", "../../res/Shaders/Standard/Cubemaps/b.fs");

    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, envCubemap, 0);
    // glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer, 0);

    std::vector<glm::mat4> cubemapCaptureTransforms(6, glm::mat4(1.0f));
    RandomHelpers::genCubeMapTransforms(0.1f, 100.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), cubemapCaptureTransforms, 0);

    equirecToCubemapShader.use();
    equirecToCubemapShader.setInt("equirectangularMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrCubeMapTex);

    // glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    // glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // for (unsigned int i = 0; i < 6; ++i)
    // {
    //     equirecToCubemapShader.setMat4("projection", cubemapCaptureTransforms[i]);
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap,
    //     0); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //     RandomHelpers::renderCube();
    // }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    for (unsigned int i = 0; i < 6; ++i)
        equirecToCubemapShader.setMat4("cubemapCaptureTransforms[" + std::to_string(i) + "]",
                                       cubemapCaptureTransforms[i]);

    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RandomHelpers::renderCube();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    cubeMapTex = envCubemap;
}
