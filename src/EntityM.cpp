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

EntityM::EntityM(const char* texturePath, glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader, const char* modelPath)
    : Entity(initialPosition,
        initialScale,
        initialShader),
    m_model(modelPath),
    m_animation(),
    m_animator(),
    hasAnimation(false),
    hasExplitcitTexture(true),
    explicitTexture(texturePath){}

EntityM::EntityM(const char* texturePath, glm::vec3& initialPosition, glm::vec3& initialScale, Shader& initialShader, const char* modelPath, glm::mat4* modelMatrices, unsigned int amount)
    : Entity(initialPosition,
        initialScale,
        initialShader),
    m_model(modelPath, modelMatrices, amount),
    m_animation(),
    m_animator(),
    hasAnimation(false),
    hasExplitcitTexture(true),
    explicitTexture(texturePath) {}

void EntityM::draw(float deltaTime, Camera& cam, bool instanced, float elapsedTime, lightDir dLight, std::vector<lightPoint>& lightPoints, glm::mat4 lightSpaceMatrix)
{
    if(hasAnimation)
        m_animator.UpdateAnimation(deltaTime);

    glm::mat4 projection = cam.GetProjectionMatrix();
    glm::mat4 view = cam.GetViewMatrix();
    
    {
        m_shader.use();
        m_shader.setInt("texture_diffuse1", 0);
        m_shader.setInt("shadowMap", 2);
        //m_shader.setInt("shadowMap", 1);

        m_shader.setMat4("projection", projection);
        m_shader.setMat4("view", view);

        //Lighting setup
        m_shader.setVec3("viewPos", cam.m_cameraPos);
        m_shader.setVec3("dirLight.direction", dLight.m_direction);
        m_shader.setVec3("dirLight.color", dLight.m_color);
        m_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);


        for (int i = 0; i < lightPoints.size(); ++i)
        {
            std::string indexStr = std::to_string(i);

            m_shader.setVec3("pointLights[" + indexStr + "].position", lightPoints[i].m_pos);
            m_shader.setVec3("pointLights[" + indexStr + "].color", lightPoints[i].m_color);
            m_shader.setFloat("pointLights[" + indexStr + "].constant", lightPoints[i].constant);
            m_shader.setFloat("pointLights[" + indexStr + "].linear", lightPoints[i].linear);
            m_shader.setFloat("pointLights[" + indexStr + "].quadratic", lightPoints[i].quadratic);

            //std::cout << "SAD" << lightPoints[i].linear << std::endl;
        }


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
        m_shader.setFloat("_Time", elapsedTime);
        
        if (hasExplitcitTexture)
        {
            explicitTexture.Bind();
            if(instanced)
            {
                m_model.DrawInstanced(m_shader);
            }
            else
            {
                m_model.Draw(m_shader);
            }
        }
        else
        {
            if (instanced)
            {
                m_model.DrawInstanced(m_shader);
            }
            else
            {
                m_model.Draw(m_shader);
            }
        }
    }    
}

void EntityM::drawDirLight(float deltaTime, Camera& cam, float elapsedTime, lightDir dLight, Shader& shader)
{
    if (hasAnimation)
        m_animator.UpdateAnimation(deltaTime);

    {
        shader.use();
        shader.setInt("texture_diffuse1", 0);
        shader.setInt("shadowMap", 2);
        //Lighting setup
        //shader.setVec3("viewPos", cam.m_cameraPos);
        


        if (hasAnimation)
        {
            auto transforms = m_animator.GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i)
                shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }



        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_position); // translate it down so it's at the center of the scene
        model = glm::scale(model, m_scale);	// it's a bit too big for our scene, so scale it down


        shader.setMat4("model", model);
        shader.setFloat("_Time", elapsedTime);

        m_model.Draw(shader);
    }

}


