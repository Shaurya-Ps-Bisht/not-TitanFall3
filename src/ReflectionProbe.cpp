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
    glGenRenderbuffers(1, &depthBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    irradianceShader = Shader("../../res/Shaders/Standard/Cubemaps/cubemap_capture.vs",
                              "../../res/Shaders/IBL/irradiance_convolution.fs",
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
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    std::vector<glm::mat4> cubemapCaptureTransforms;
    RandomHelpers::genCubeMapTransforms(1.0f, 10.0f, 1.0f, pos, cubemapCaptureTransforms, 0);

    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);

    glViewport(0, 0, 32, 32);
    for (unsigned int i = 0; i < 6; ++i)
        irradianceShader.setMat4("cubemapCaptureTransforms[" + std::to_string(i) + "]", cubemapCaptureTransforms[i]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RandomHelpers::renderCube();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return irradianceMap;
}
