#pragma once
#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const unsigned int NUM_PATCH_PTS = 4;


class Terrain
{
public:
	Terrain(const char* mapPath);
	~Terrain();

	void Draw(Camera& camera);
	void LoadFromFile(const char* filename);

public:
	unsigned int m_textureID;
	unsigned int m_terrainVAO, m_terrainVBO;


private:




private:
	Shader m_terrainShader;
	unsigned rez;
	float m_ResolutionWidth, m_ResolutionHeight;
};


#endif