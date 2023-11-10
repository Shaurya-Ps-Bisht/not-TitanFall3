#pragma once
#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "Shader.h"
#include "Texture.h"

class Terrain
{
public:
	Terrain();
	~Terrain();

	void Draw();
	void LoadFromFile(const char* filename);

private:




private:
	Shader m_terrainShader;
	Texture m_terrainTexture;


};


#endif