#include "lightDir.h"

glm::mat4 lightDir::getLightSpaceMatrix(const float& nearPlane, const float& farPlane, const glm::mat4& view, float FOV, float aspectRatio)
{
    const auto proj = glm::perspective(
        glm::radians(FOV), (float)aspectRatio, nearPlane,
        farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj, view);

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center - m_direction, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();

    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }


    constexpr float zMult = 5.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;

}

std::vector<glm::mat4> lightDir::getLightSpaceMatrices(float nearPlane, float farPlane, const glm::mat4& view, float FOV, float aspectRatio)
{
    m_shadowCascadeLevels = { farPlane / 150.0f, farPlane / 25.0f, farPlane / 10.0f, farPlane / 2.0f };

    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < m_shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(nearPlane, m_shadowCascadeLevels[i], view, FOV, aspectRatio));
        }
        else if (i < m_shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(m_shadowCascadeLevels[i - 1], m_shadowCascadeLevels[i],  view, FOV, aspectRatio));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(m_shadowCascadeLevels[i - 1], farPlane, view, FOV, aspectRatio));
        }
    }
    return ret;

}

std::vector<glm::vec4> lightDir::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    return getFrustumCornersWorldSpace(proj * view);

}

std::vector<glm::vec4> lightDir::getFrustumCornersWorldSpace(const glm::mat4& projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

void lightDir::configureLightFBO()
{
    glGenFramebuffers(1, &m_lightFBO);

    glGenTextures(1, &m_lightDepthMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_lightDepthMaps);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, m_depthMapResolution, m_depthMapResolution, 5,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_lightDepthMaps, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
        throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
