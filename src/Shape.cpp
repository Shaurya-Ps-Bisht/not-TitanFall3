#include "Shape.h"


class Quad : public Shape
{
public:
	virtual void initVertexData() override
	{
		m_vertices.push_back(Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		m_vertices.push_back(Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		m_vertices.push_back(Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		m_vertices.push_back(Vertex(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

		m_indices.insert(m_indices.end(), { 0, 1, 2, 2, 3, 0 });
	}
};

class Cube : public Shape
{
	virtual void initVertexData() override
	{
		std::vector<std::vector<glm::vec3>> facePositions = {
			{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f)},  // Front
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f)},  // Back
			{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f)},  // Right
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, -0.5f)},  // Left
			{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f)},  // Top
			{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f)}   // Bottom
		};

		std::vector<glm::vec2> textures = {glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)};

		for (const auto& facePos : facePositions) {
			for (int i = 0; i < 4; ++i) {
				int textureIndex = i % 4;
				int nextTextureIndex = (i + 1) % 4;

				m_vertices.push_back(Vertex(facePos[i], textures[textureIndex], CalculateNormal(facePos)));
			}
		}
		m_indices = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20 };
	}

	glm::vec3 CalculateNormal(const std::vector<glm::vec3>& vertices) {
		glm::vec3 edge1 = vertices[1] - vertices[0];
		glm::vec3 edge2 = vertices[2] - vertices[0];
		return glm::normalize(glm::cross(edge1, edge2));
	}
};

class Sphere : public Shape
{
	virtual void initVertexData() override
	{
		float sectorStep = 2 * m_PI / m_sectors;
		float stackStep = m_PI / m_stacks;

		// Iterate over stacks
		for (int i = 0; i <= m_stacks; ++i) {
			float stackAngle = m_PI / 2 - i * stackStep;
			float xy = m_radius * cosf(stackAngle);
			float z = m_radius * sinf(stackAngle);

			// Add vertices for the current stack
			for (int j = 0; j <= m_sectors; ++j) {
				float sectorAngle = j * sectorStep;

				// Calculate vertex position
				float x = xy * cosf(sectorAngle);
				float y = xy * sinf(sectorAngle);


				// Calculate normalized vertex normal
				float lengthInv = 1.0f / m_radius;
				float nx = x * lengthInv;
				float ny = y * lengthInv;
				float nz = z * lengthInv;

				// Calculate vertex texture coordinates
				float s = static_cast<float>(j) / m_sectors;
				float t = static_cast<float>(i) / m_stacks;

				m_vertices.push_back(Vertex(glm::vec3(x, y, z), glm::vec2(s,t), glm::vec3(nx, ny, nz)));
			}
		}

		// Generate triangle indices
		for (int i = 0; i < m_stacks; ++i) {
			int k1 = i * (m_sectors + 1);
			int k2 = k1 + m_sectors + 1;

			for (int j = 0; j < m_sectors; ++j, ++k1, ++k2) {
				// Two triangles per sector except for first and last stacks
				if (i > 0) {
					m_indices.push_back(k1);
					m_indices.push_back(k2);
					m_indices.push_back(k1 + 1);
				}
				if (i < m_stacks - 1) {
					m_indices.push_back(k1 + 1);
					m_indices.push_back(k2);
					m_indices.push_back(k2 + 1);
				}
			}
		}
	}

	

private:
	int m_stacks = 75;
	int m_sectors = 75;
	int m_radius = 3;
	const float m_PI = 3.14159265359f;
};

Shape* Shape::FetchShape(const std::string& shape)
{
	if (shape == "CUBE") {
		return new Cube();
	}
	else if (shape == "SPHERE") {
		return new Sphere();
	}
	else if (shape == "QUAD") {
		return new Quad();
	}

	return nullptr;
}