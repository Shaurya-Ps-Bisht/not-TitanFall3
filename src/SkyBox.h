#pragma once

#include <vector>
#include <string>
#include <stb_image.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Camera;
class Shader;
class SkyBox
{
  public:
    SkyBox(){};
    SkyBox(const char *path);
    ~SkyBox(){};

    void draw(Camera &cam, glm::vec3(color));
    

  private:
    unsigned int loadCubemap(std::vector<std::string> faces);
    unsigned int loadCubemapHDR(const std::string &filePath);

    void equirecToCubemap();

  private:
    Shader *skyboxShader, *hdrSkyboxShader;
    unsigned int skyboxVAO, skyboxVBO, hdrCubeMapTex;
    unsigned int frameBuffer;
    unsigned int depthBuffer;
    unsigned int cubeMapTex;
};
