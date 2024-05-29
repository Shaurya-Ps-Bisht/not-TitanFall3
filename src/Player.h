#pragma once
#ifndef __PLAYER_H__
#define __PLAYER_H__

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include "Animation.h"
#include "Animator.h"
#include "lightDir.h"
#include "lightPoint.h"
#include "ShadowManager.h"

#include "Camera.h"
#include "EntityM.h"

#include <memory>

class Player
{
public:
	~Player();

	static Player& GetInstance() {
		static Player instance;
		return instance;
	}

	void Draw(float dt, Camera& cam, lightDir dLight, std::vector<lightPoint>& lightPoints);
	void UpdatePlayerPos(glm::vec3& newPos);
	void UpdatePlayerRotation(float x, float y);
	void InitPlayer(Camera& cam);
    std::shared_ptr<EntityM> m_playerModel;


private:
	void SetPlayerAnimations();
	void SetPlayerModel();

private:
	bool m_moving, m_flashlightEnabled;

	Shader m_playerShader;
	glm::vec3 m_playerPos;
	Animation m_animations;
	Animator m_animator;
	//Model m_playerModel;


	float m_horizontalRotation = -90;
};


#endif