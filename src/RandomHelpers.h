#pragma once

#ifndef __RANDOMHELPERS_H__
#define __RANDOMHELPERS_H__
#include <GLFW/glfw3.h>

#include <array>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

struct Plane
{
    glm::vec3 normal = {0.f, 1.f, 0.f};
    float distance = 0.f;

    Plane() = default;

    Plane(const glm::vec3 &p1, const glm::vec3 &norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, p1))
    {
    }

    float getSignedDistanceToPlane(const glm::vec3 &point) const
    {
        return glm::dot(normal, point) - distance;
    }
};

struct Frustum
{
    Plane topFace;
    Plane bottomFace;
    Plane rightFace;
    Plane leftFace;
    Plane farFace;
    Plane nearFace;
};

struct BoundingVolume
{
    virtual bool isOnOrForwardPlane(const Plane &plane) const = 0;

    bool isOnFrustum(const Frustum &camFrustum) const
    {
        return (isOnOrForwardPlane(camFrustum.leftFace) && isOnOrForwardPlane(camFrustum.rightFace) &&
                isOnOrForwardPlane(camFrustum.topFace) && isOnOrForwardPlane(camFrustum.bottomFace) &&
                isOnOrForwardPlane(camFrustum.nearFace) && isOnOrForwardPlane(camFrustum.farFace));
    };
};

struct AABB : public BoundingVolume
{
    glm::vec3 center{0.f, 0.f, 0.f};
    glm::vec3 extents{0.f, 0.f, 0.f};

    AABB(const glm::vec3 &min, const glm::vec3 &max)
        : BoundingVolume{}, center{(max + min) * 0.5f}, extents{max.x - center.x, max.y - center.y, max.z - center.z}
    {
    }

    AABB(const glm::vec3 &inCenter, float iI, float iJ, float iK)
        : BoundingVolume{}, center{inCenter}, extents{iI, iJ, iK}
    {
    }

    std::array<glm::vec3, 8> getAABBVertices() const
    {
        std::array<glm::vec3, 8> vertices;
        vertices[0] = {center.x - extents.x, center.y - extents.y, center.z - extents.z};
        vertices[1] = {center.x + extents.x, center.y - extents.y, center.z - extents.z};
        vertices[2] = {center.x - extents.x, center.y + extents.y, center.z - extents.z};
        vertices[3] = {center.x + extents.x, center.y + extents.y, center.z - extents.z};
        vertices[4] = {center.x - extents.x, center.y - extents.y, center.z + extents.z};
        vertices[5] = {center.x + extents.x, center.y - extents.y, center.z + extents.z};
        vertices[6] = {center.x - extents.x, center.y + extents.y, center.z + extents.z};
        vertices[7] = {center.x + extents.x, center.y + extents.y, center.z + extents.z};
        return vertices;
    }

    bool isOnOrForwardPlane(const Plane &plane) const final
    {
        const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
                        extents.z * std::abs(plane.normal.z);

        return -r <= plane.getSignedDistanceToPlane(center);
    }
};

namespace RandomHelpers
{

float getHeight(float x, float z, const unsigned char *data, const int &m_ResolutionWidth,
                const int &m_ResolutionHeight);

glm::mat4 *instanceMatrixTerrain(unsigned int amount, float radius, float offset, float startAngle, float endAngle,
                                 const unsigned char *data, const int &m_ResolutionWidth,
                                 const int &m_ResolutionHeight);
} // namespace RandomHelpers

#endif
