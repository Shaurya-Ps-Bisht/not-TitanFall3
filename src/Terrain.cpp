#include "Terrain.h"

Terrain::Terrain(const char* mapPath)
{
    GLint maxTessLevel;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    std::cout << "Max available tess level: " << maxTessLevel << std::endl;

    // build and compile our shader program
    // ------------------------------------
    m_terrainShader = Shader("res/Shaders/Terrain/terrain.vs",
        "res/Shaders/Terrain/terrain.fs", nullptr,
        "res/Shaders/Terrain/terrain.tcs",
        "res/Shaders/Terrain/terrain.tes");

    // load and create a texture
    // -------------------------
    glGenTextures(1, &m_textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID); 
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load(mapPath, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        m_terrainShader.setInt("heightMap", 0);
        std::cout << "Shader " << m_terrainShader.m_ID<< std::endl;
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // ------------------------------------------------------------------
    std::vector<float> vertices;

    rez = 20;
    for (unsigned i = 0; i <= rez - 1; i++)
    {
        for (unsigned j = 0; j <= rez - 1; j++)
        {
            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back(j / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back(i / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back((j + 1) / (float)rez); // v
        }
    }
    std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
    std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

    // first, configure the cube's VAO (and terrainVBO)
    glGenVertexArrays(1, &m_terrainVAO);
    glBindVertexArray(m_terrainVAO);

    glGenBuffers(1, &m_terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

}

Terrain::~Terrain()
{
}

void Terrain::Draw(Camera& camera)
{
    m_terrainShader.use();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.m_FOV), (float)1440 / (float)900, 0.01f, 1000000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    m_terrainShader.setMat4("projection", projection);
    m_terrainShader.setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    m_terrainShader.setMat4("model", model);
    std::cout << "While Drawing: " << m_terrainShader.m_ID << std::endl;


    // render the terrain
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glBindVertexArray(m_terrainVAO);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
}

void Terrain::LoadFromFile(const char* filename)
{
}
