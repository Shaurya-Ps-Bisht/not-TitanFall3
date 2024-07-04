#pragma once
#include <vector>
#ifndef __REFLECTION_PROBE__
#define __REFLECTION_PROBE__
#define NOMINMAX
#define MAX_PROBES 4

#include "glm.hpp"

class ReflectionProbe
{
    static ReflectionProbe &getInstance()
    {
        static ReflectionProbe instance;
        return instance;
    }

    void addProbe(const glm::vec3 &position);
    void bake(unsigned int index);
    void checkAndBindCubemapArray(const glm::vec3 &objectPosition);

  private:
    ReflectionProbe();
    bool isCubemapInArray(int index);
    int findClosestProbe(const glm::vec3 &objectPosition);

    std::vector<glm::vec3> probePositions;
    std::vector<unsigned int> probeCubemaps;
    unsigned int cubemapArray;
    unsigned int framebuffer;
    unsigned int depthBuffer;
    const unsigned int CUBEMAP_RESOLUTION = 512;
    int currentLayer = 0;
};

#endif // !__REFLECTION_PROBE__
