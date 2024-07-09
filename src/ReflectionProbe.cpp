#include "ReflectionProbe.h"
#include "RandomHelpers.h"
#include <glad.h>
#include <limits>

void ReflectionProbe::addProbe(const glm::vec3 &position)
{
    probePositions.push_back(position);
    unsigned int cubemapTextureID;
    glGenTextures(1, &cubemapTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA16F, 1024, 1024);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    probeCubemaps.push_back(cubemapTextureID);
    bake(probeCubemaps.size() - 1);
}

void ReflectionProbe::bake(unsigned int index) {

};
void ReflectionProbe::checkAndBindCubemapArray(const glm::vec3 &objectPosition)
{
    int closestProbeIndex = findClosestProbe(objectPosition);

    if (!isCubemapInArray(closestProbeIndex))
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubemapArray);
        glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, currentLayer * 6, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION, 6,
                        GL_RGBA, GL_FLOAT, nullptr);
    }
}

ReflectionProbe::ReflectionProbe()
{
    glGenTextures(1, &cubemapArray);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubemapArray);
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_RGBA16F, 1024, 1024, 6 * MAX_PROBES);

    // generic use frameRenderBuffer generation
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //     glGenTextures(1, &depthBuffer);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, depthBuffer);
    // for (unsigned int i = 0; i < 6; ++i)
    // {
    //     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, 32, 32, 0,
    //     GL_DEPTH_COMPONENT,
    //                  GL_FLOAT, nullptr);
    // }
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    irradianceShader = Shader("../../res/Shaders/Standard/Cubemaps/cubemap_capture.vs",
                              "../../res/Shaders/IBL/irradiance_convolution.fs",
                              "../../res/Shaders/Standard/Cubemaps/cubemap_capture.gs");
    prefilterShader =
        Shader("../../res/Shaders/Standard/Cubemaps/cubemap_capture.vs", "../../res/Shaders/IBL/prefilter.fs",
               "../../res/Shaders/Standard/Cubemaps/cubemap_capture.gs");
}

bool ReflectionProbe::isCubemapInArray(int index)
{
    return false;
}

int ReflectionProbe::findClosestProbe(const glm::vec3 &objectPosition)
{
    float minDistance = std::numeric_limits<float>::max();
    int closestIndex = -1;

    for (unsigned int i = 0; i < probePositions.size(); ++i)
    {
        float distance = glm::distance(objectPosition, probePositions[i]);
        if (distance < minDistance)
        {
            minDistance = distance;
            closestIndex = i;
        }
    }

    return closestIndex;
}

void ReflectionProbe::generateSkyBoxIrradianceMap(const unsigned int &cubemapId)
{
    brdfMap = generateBrdfMap();
    skyIrrMap = generateIrradianceMap(cubemapId, glm::vec3(0, 0, 0));
    skyPrefilterMap = generatePrefilterMap(cubemapId, glm::vec3(0, 0, 0));
}

unsigned int ReflectionProbe::generateIrradianceMap(const unsigned int &cubemapId, glm::vec3 pos)
{

    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, irradianceMap, 0);

    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);

    glViewport(0, 0, 32, 32);

    std::vector<glm::mat4> cubemapCaptureTransforms(6, glm::mat4(1.0f));
    RandomHelpers::genCubeMapTransforms(1.0f, 10.0f, 1.0f, pos, cubemapCaptureTransforms, 0);

    for (unsigned int i = 0; i < 6; ++i)
        irradianceShader.setMat4("cubemapCaptureTransforms[" + std::to_string(i) + "]", cubemapCaptureTransforms[i]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RandomHelpers::renderCube();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return irradianceMap;
}
unsigned int ReflectionProbe::generatePrefilterMap(const unsigned int &cubemapId, glm::vec3 pos)
{
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);

    std::vector<glm::mat4> cubemapCaptureTransforms(6, glm::mat4(1.0f));
    RandomHelpers::genCubeMapTransforms(1.0f, 10.0f, 1.0f, pos, cubemapCaptureTransforms, 0);

    for (unsigned int i = 0; i < 6; ++i)
        prefilterShader.setMat4("cubemapCaptureTransforms[" + std::to_string(i) + "]", cubemapCaptureTransforms[i]);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, prefilterMap, mip);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RandomHelpers::renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return prefilterMap;
}

unsigned int ReflectionProbe::generateBrdfMap()
{
    glDisable(GL_BLEND);
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    brdfShader = Shader("../../res/Shaders/IBL/brdf.vs", "../../res/Shaders/IBL/brdf.fs");

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    // glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT);
    RandomHelpers::renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_BLEND);

    return brdfLUTTexture;
}
