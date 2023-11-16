#pragma once
#ifndef  __VERTEX_H__
#define __VERTEX_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Vertex
{
public:
	Vertex() {};
	Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normals)
		: m_pos(pos),
		m_tex(tex),
		m_normals(normals) {}


	glm::vec3& GetPos() { return m_pos; }
	glm::vec2& GetTex() { return m_tex; }
	glm::vec3& GetNormals() { return m_normals; }

private:
	glm::vec3 m_pos;
	glm::vec2 m_tex;
	glm::vec3 m_normals;
};

#endif 