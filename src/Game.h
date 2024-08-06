#pragma once
#ifndef __GAME_H__
#define __GAME_H__
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Animation.h"
#include "Animator.h"
#include "Camera.h"
#include "EntityM.h"
#include "EntityTerrain.h"
#include "EntityV.h"
#include "Model.h"
#include "Player.h"
#include "RandomHelpers.h"
#include "Renderer.h"
#include "Shader.h"
#include "ShadowManager.h"
#include "SkyBox.h"
#include "ReflectionProbe.h"
#include "Texture.h"
#include "lightDir.h"
#include "lightPoint.h"

//#include <irrklang/irrKlang.h>

//using namespace irrklang;
using EntityPtr = std::variant<std::shared_ptr<EntityM>, std::unique_ptr<EntityM>, std::unique_ptr<EntityV>,
                               std::unique_ptr<EntityTerrain>>;

class Game
{
  public:
    Game();
    ~Game();

  public:
    GLFWwindow *m_window;
    Camera m_camera;

    void Run();

  private:
    void GameLoop();
    void RenderLoop();
    void RtLoop();

    void initEntities();
    void initData();
    void processInput(GLFWwindow *window);
    void stateCheck();

    void drawCascadeVolumeVisualizers(const std::vector<glm::mat4> &lightMatrices, Shader *shader);

  public:
    bool godMode;

  private:
    int level = 0;
    bool RayTracer = true;

    bool playerRuning = false;
    float m_deltaTime;

    unsigned int m_skyTexture;
    std::vector<std::shared_ptr<Entity>> m_entitiesShared;
    std::vector<std::unique_ptr<Entity>> m_entitiesUnique;
    std::vector<std::unique_ptr<Entity>> m_entitiesInstanced;
    std::vector<EntityPtr> m_entities;

    SkyBox m_skyBox;
    Shader m_skyShader;
    Shader debugDepthQuad;
    Shader hdrShader;
    Shader shaderBlur;


    std::unique_ptr<EntityTerrain> m_terrain;

    unsigned char *data;
    int m_ResolutionWidth, m_ResolutionHeight, m_nrChannels;

    int debugLayer;

    std::vector<GLuint> visualizerVAOs;
    std::vector<GLuint> visualizerVBOs;
    std::vector<GLuint> visualizerEBOs;

    unsigned int hdrFBO;
    unsigned int colorBuffers[2];
    unsigned int rboDepth;

    bool hdr = true;
    bool hdrKeyPressed = false;
    float exposure = 1.0f;

    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];


};

#endif
