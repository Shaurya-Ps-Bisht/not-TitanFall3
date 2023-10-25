#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <GLAD/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:

	Shader(const char* vertexPath, const char* fragmentPath);
	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

private:
	unsigned int m_ID;

	void checkCompileErrors(unsigned int shader, std::string type);

};



#endif