#include "ReflectionProbe.h"
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

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
