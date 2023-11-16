#pragma once
#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "Vertex.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

class Shape
{
public:
	static Shape* FetchShape(const std::string& shapeType);

public:
	virtual std::vector<Vertex> GetVertexData() = 0;
	virtual unsigned int GetVertexDataCount() = 0;
	virtual unsigned int* GetIndexData() = 0;
	virtual unsigned int GetIndexDataCount() = 0;
	virtual void InitVertexData() = 0;
};



#endif 