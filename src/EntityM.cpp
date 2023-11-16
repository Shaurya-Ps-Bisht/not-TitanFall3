#include "EntityM.h"

EntityM::EntityM(glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader, const char* modelPath)
    : Entity(initialPosition,
        initialScale,
        initialShader),
    m_model(modelPath),
    hasAnimation(false)
     {}

EntityM::EntityM(glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader, const char* modelPath, const char* animationName)
    : Entity(initialPosition,
        initialScale,
        initialShader),
    m_model(modelPath),
    m_animation(modelPath, &m_model, animationName),
    m_animator(&m_animation),
    hasAnimation(true){}

void EntityM::draw(float deltaTime, Camera& cam)
{
    if(hasAnimation)
        m_animator.UpdateAnimation(deltaTime);

    glm::mat4 projection = cam.GetProjectionMatrix();
    glm::mat4 view = cam.GetViewMatrix();
    
    {
        m_shader.use();
        m_shader.setMat4("projection", projection);
        m_shader.setMat4("view", view);

        if(hasAnimation)
        {
            auto transforms = m_animator.GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i)
                m_shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }



        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_position); // translate it down so it's at the center of the scene
        model = glm::scale(model, m_scale);	// it's a bit too big for our scene, so scale it down


        m_shader.setMat4("model", model);
        m_model.Draw(m_shader);
    }    
}


