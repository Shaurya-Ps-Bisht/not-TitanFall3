#pragma once
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Animation.h"
#include "Animator.h"

class Player
{
public:
	Player();
	~Player();

	static Player& GetInstance() {
		static Player instance;
		return instance;
	}

private:
	void SetPlayerAnimations();
	void SetPlayerModel();

private:
	bool m_moving, m_flashlightEnabled;

	glm::vec3 m_pos;
	Model m_playerModel;
	Animation* m_animations;
	Animator m_animator;
};


#endif