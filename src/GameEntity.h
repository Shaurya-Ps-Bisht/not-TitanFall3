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
	GameEntity(const glm::vec3& pos, const glm::vec3& scale, const Shader& shader);
	~GameEntity();

	virtual void draw() const = 0;
	void changePosition(const glm::vec3& newPosition) { position = newPosition; }
	void changeScale(const glm::vec3& newScale) { scale = newScale; }
	//void InstancedDraw();



private:
	glm::vec3 position;
	glm::vec3 scale;

	//Transform m_transform;
	Shader shaderComponent;
	//Animation m_animationComponent;

};

#endif