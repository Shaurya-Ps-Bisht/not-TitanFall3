#pragma once
#include <vector>
#ifndef __REFLECTION_PROBE__
#define __REFLECTION_PROBE__


#include "glm.hpp"


class ReflectionProbe{
    static ReflectionProbe& getInstance() {
        static ReflectionProbe instance;
        return instance;
    }

    void addProbe(const glm::vec3 &position);
    void bake(unsigned int index);
    void checkAndBindCubemapArray(const glm::vec3 &objectPosition);

    private:
      ReflectionProbe();
      bool isCubemapInArray(int index);

      std::vector<glm::vec3> pos;
      std::vector<unsigned int> probe_cubemaps;
      unsigned int cubemaparray;
      const unsigned int CUBEMAP_RESOLUTION = 512;
      int currentLayer = 0;



  };

#endif // !__REFLECTION_PROBE__

