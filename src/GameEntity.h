#pragma once
#ifndef __GAMEENTITY__
#define __GAMEENTITY__

#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class GameEntity
{
public:
	GameEntity();
	~GameEntity();

	void CreateEntity();
	void Draw();
	void InstancedDraw();



private:
	glm::mat4* m_modelMatricesIns;
	//Transform m_transform;
	Shader shaderComponent;
	Texture m_textureComponent;
	//Animation m_animationComponent;

};

#endif