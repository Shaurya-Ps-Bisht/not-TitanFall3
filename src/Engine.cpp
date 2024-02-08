#include "Engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::Run()
{
	Renderer::GetInstance().Run();
	m_game.m_window = Renderer::GetInstance().m_window;
	m_game.Run();
}

