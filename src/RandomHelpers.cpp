#include "RandomHelpers.h"

float RandomHelpers::getHeight(float x, float z, const unsigned char *data, const int &m_ResolutionWidth,
                               const int &m_ResolutionHeight)
{
    if (data)
    {
        x += m_ResolutionWidth / 2;
        z += m_ResolutionHeight / 2;

        if (x < 0 || z < 0 || x >= m_ResolutionWidth - 1 || z >= m_ResolutionHeight - 1)
        {
            return -100.0f; // Adjusted boundary conditions
        }

        // Get the integer coordinates of the top-left corner of the cell
        int x0 = (int)x;
        int z0 = (int)z;

        // Calculate the fractional part of the coordinates
        float dx = x - (float)x0;
        float dz = z - (float)z0;

        // Calculate the heights at the four corners of the cell
        int index00 = ((int)x0 + (int)m_ResolutionWidth * (int)z0) * 4;
        int index10 = ((int)(x0 + 1) + (int)m_ResolutionWidth * (int)z0) * 4;
        int index01 = ((int)x0 + (int)m_ResolutionWidth * (int)(z0 + 1)) * 4;
        int index11 = ((int)(x0 + 1) + (int)m_ResolutionWidth * (int)(z0 + 1)) * 4;

        const unsigned char *offset00 = data + index00;
        const unsigned char *offset10 = data + index10;
        const unsigned char *offset01 = data + index01;
        const unsigned char *offset11 = data + index11;

        // Bilinear interpolation
        float h00 = ((float)offset00[0] / 255.0f) * 100.0f - 16.0f;
        float h10 = ((float)offset10[0] / 255.0f) * 100.0f - 16.0f;
        float h01 = ((float)offset01[0] / 255.0f) * 100.0f - 16.0f;
        float h11 = ((float)offset11[0] / 255.0f) * 100.0f - 16.0f;

        float finalHeight = (1.0f - dz) * ((1.0f - dx) * h00 + dx * h10) + dz * ((1.0f - dx) * h01 + dx * h11);

        return finalHeight;
    }
    else
    {
        return 0.0f;
    }
}
glm::mat4 *RandomHelpers::instanceMatrixTerrain(unsigned int amount, float radius, float offset, float startAngle,
                                                float endAngle, const unsigned char *data, const int &m_ResolutionWidth,
                                                const int &m_ResolutionHeight)
{
    glm::mat4 *modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed

    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);

        float angle = glm::mix(startAngle, endAngle, static_cast<float>(i) / static_cast<float>(amount - 1));
        float displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float x = -sin(glm::radians(angle)) * radius + displacement;

        displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float z = -cos(glm::radians(angle)) * radius + displacement;
        x += 490.0f;
        z += 750.0f;
        float y = getHeight(x, z, data, m_ResolutionWidth, m_ResolutionHeight) + 1.0f;

        model = glm::translate(model, glm::vec3(x, y, z));

        float scale = 0.01f;

        model = glm::scale(model, glm::vec3(scale));

        float rotAngle = static_cast<float>((rand() % 360));

        modelMatrices[i] = model;
    }

    return modelMatrices;
}
