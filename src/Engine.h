#pragma once
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Game.h"
#include "Renderer.h"


class Engine 
{
public:
	Engine();
	~Engine();

	void Run();

private:
	Game m_game;

};

#endif

