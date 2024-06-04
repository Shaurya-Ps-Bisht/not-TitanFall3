#pragma once
#ifndef __SHAPE_H__
#define __SHAPE_H__

//#include "Vertex.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


class Shape
{

public:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec2 tex;
        glm::vec3 normals;

        Vertex(const glm::vec3& position, const glm::vec2& texture, const glm::vec3& normal)
            : pos(position), tex(texture), normals(normal) {}
    };


public:
	static Shape* FetchShape(const std::string& shapeType);

public:

	virtual void initVertexData() = 0;

	std::vector<Vertex> getVertex() 
	{
		return m_vertices;
	}

	unsigned int getVertexDataCount() 
	{
		return m_vertices.size();
	}

    std::vector<unsigned int> getIndexData() {
        return m_indices;
    };

	unsigned int getIndexDataCount() 
	{
		return m_indices.size();
	}


protected:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
};



#endif 