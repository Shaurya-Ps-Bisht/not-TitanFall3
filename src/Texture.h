#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <string>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>
#include <vector>

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};

#endif