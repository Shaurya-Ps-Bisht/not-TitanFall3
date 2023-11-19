#pragma once

#include <vector>

#include <stb_image.h>

#include "Shader.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class SkyBox
{
public:
	SkyBox() {};
	SkyBox(const char* path);
	~SkyBox(){};

	void draw(Camera& cam, glm::vec3(color));

private:
	unsigned int loadCubemap(std::vector<std::string> faces);

private:
	Shader skyboxShader;
	unsigned int skyboxVAO, skyboxVBO, cubeMapTex;
};