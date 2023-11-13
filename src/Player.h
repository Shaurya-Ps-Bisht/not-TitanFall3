#pragma once
#ifndef __PLAYER_H__
#define __PLAYER_H__

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include "Animation.h"
#include "Animator.h"

#include "Camera.h"

class Player
{
public:
	~Player();

	static Player& GetInstance() {
		static Player instance;
		return instance;
	}

	void Draw(float dt, Camera& cam, Animator& anim);
	void UpdatePlayerPos(glm::vec3 posOffset);
	void UpdatePlayerRotation(float x, float y);
	void InitPlayer();


private:
	void SetPlayerAnimations();
	void SetPlayerModel();

private:
	bool m_moving, m_flashlightEnabled;

	glm::vec3 m_pos;
	float m_horizontalRotation = -90;
	Model m_playerModel;
	Animation m_animations;
	Animator m_animator;
	Shader m_playerShader;
};


#endif