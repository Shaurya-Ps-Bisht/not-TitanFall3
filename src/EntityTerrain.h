#pragma once
#ifndef __ENTITY_TERRAIN_H__
#define __ENTITY_TERRAIN_H__

#include "Shader.h"
#include "Texture.h"
#include "Entity.h"


#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "lightDir.h"

const unsigned int NUM_PATCH_PTS = 4;


class EntityTerrain : public Entity {
public:
    EntityTerrain(const std::string &name, const unsigned char *data, const int &width, const int &height);
	~EntityTerrain();

	void draw(const float &deltaTime, Camera &cam, bool instanced, float elapsedTime, lightDir dLight,
              std::vector<lightPoint> &lightPoints, glm::mat4 lightSpaceMatrix);
    void drawDirLight(const float &deltaTime, bool instanced, Camera &cam, float elapsedTime, lightDir dLight,
                      Shader &shader);
	void LoadFromFile(const char* filename);

public:
	unsigned int m_textureID;
	unsigned int m_terrainVAO, m_terrainVBO;
	

private:
	void loadSand();



private:
	
	unsigned rez;
	unsigned int m_sand;
    int m_ResolutionWidth, m_ResolutionHeight;
};


#endif