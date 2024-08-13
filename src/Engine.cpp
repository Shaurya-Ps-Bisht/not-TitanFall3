#include "Engine.h"
#include "Game.h"
#include "Renderer.h"


Engine::Engine()
{
    m_game = new Game();
}

Engine::~Engine()
{
}

void Engine::Run()
{
    Renderer::GetInstance().Run();
    m_game->m_window = Renderer::GetInstance().m_window;
    m_game->Run();
}
