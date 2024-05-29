#pragma once
#ifndef __ENTITY_M__
#define __ENTITY_M__


#include "Entity.h"

class EntityM : public Entity {

public:
    // Constructor for objects without animation
  EntityM(const std::string &name, glm::vec3 &initialPosition, glm::vec3 &initialScale, Shader &initialShader,
          const char *modelPath);
    // Constructor for objects with animation
  EntityM(const std::string &name, glm::vec3 &initialPosition, glm::vec3 &initialScale,
          Shader &initialShader,
          const char *modelPath, const char *animationName);
    //Constructors for objects with explicit textures
  EntityM(const std::string &name, const char *texturePath, glm::vec3 &initialPosition, glm::vec3 &initialScale,
          Shader &initialShader, const char *modelPath);
    //Constructors for objects with explicit textures and instanced rendering
  EntityM(const std::string &name, const char *texturePath, glm::vec3 &initialPosition, glm::vec3 &initialScale,
          Shader &initialShader, const char *modelPath, glm::mat4 *modelMatrices, unsigned int amount);
    //and this is the destructorrr
    ~EntityM() {};
    // Destructor to clean up optional objects



    // Draw method
    void draw(const float &deltaTime, Camera &cam, bool instanced, float elapsedTime, lightDir dLight,
              std::vector<lightPoint> &lightPoints, glm::mat4 lightSpaceMatrix);
    void drawDirLight(const float &deltaTime, bool instanced, Camera &cam, float elapsedTime, lightDir dLight,
                      Shader &shader);
    
    Model m_model;

private:
    Animation m_animation;
    Animator m_animator; 

    bool hasAnimation = false;
    bool hasExplitcitTexture = false;
    Texture explicitTexture;
    
};

#endif