#include "GameEntity.h"

GameEntity::GameEntity(glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader)
	: m_position(initialPosition), m_scale(initialScale), m_shader(initialShader) {}
