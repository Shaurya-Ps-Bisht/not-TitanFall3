#pragma once
#include "Shader.h"
#include <vector>

#ifndef __REFLECTION_PROBE__
#define __REFLECTION_PROBE__
#define NOMINMAX
#define MAX_PROBES 4

#include "glm.hpp"

class ReflectionProbe
{
  public:
    static ReflectionProbe &GetInstance()
    {
        static ReflectionProbe instance;
        return instance;
    }

    void addProbe(const glm::vec3 &position);
    void bake(unsigned int index);
    void checkAndBindCubemapArray(const glm::vec3 &objectPosition);
    unsigned int generateIrradianceMap(const unsigned int &cubemapId, glm::vec3 pos);
    unsigned int generatePrefilterMap(const unsigned int &cubemapId, glm::vec3 pos);
    unsigned int generateBrdfMap();
    void generateSkyBoxIrradianceMap(const unsigned int &cubemapId);

  public:
    unsigned int skyIrrMap;
    unsigned int skyPrefilterMap;
    unsigned int brdfMap;

  private:
    ReflectionProbe();
    bool isCubemapInArray(int index);
    int findClosestProbe(const glm::vec3 &objectPosition);

    std::vector<glm::vec3> probePositions;
    std::vector<unsigned int> probeCubemaps;
    unsigned int cubemapArray;
    unsigned int frameBuffer;
    unsigned int depthBuffer;
    const unsigned int CUBEMAP_RESOLUTION = 512;
    int currentLayer = 0;

    Shader irradianceShader, prefilterShader, brdfShader;
};

#endif // !__REFLECTION_PROBE__
