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
    data = stbi_load(mapPath, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        m_terrainShader.setInt("heightMap", 0);
        std::cout << "Shader " << m_terrainShader.m_ID << std::endl;

        m_ResolutionWidth = width;
        m_ResolutionHeight = height;
        m_nrChannels = nrChannels;

        std::cout << "Loaded heightmap of size " << m_ResolutionWidth << " x " << m_ResolutionHeight <<" CHANNELS: "<< m_nrChannels <<std::endl;

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
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
    stbi_image_free(data);

}

void Terrain::Draw(Camera& camera)
{
    int widthUniformLocation = glGetUniformLocation(m_terrainShader.m_ID, "ourColor");
    m_terrainShader.use();
    m_terrainShader.setVec2("uTexelSize", 1 / m_ResolutionWidth, 1 / m_ResolutionHeight);
    //m_terrainShader.setInt("uHeightMap", 0);


    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.m_FOV), (float)1440 / (float)900, 0.01f, 1000000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    m_terrainShader.setMat4("projection", projection);
    m_terrainShader.setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    m_terrainShader.setMat4("model", model);

    // render the 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glBindVertexArray(m_terrainVAO);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
}

void Terrain::LoadFromFile(const char* filename)
{
    
}

float Terrain::getHeight(float x, float z)
{
    if(data)
    {
        x += m_ResolutionWidth/2;
        z += m_ResolutionHeight/2;

        int index = ((int)x + (int)m_ResolutionWidth* (int)z) * 4;
        // Access the pixel values at the specified position
        unsigned char* offset = data + index;

        float final = (float)offset[0]/255 * 64.0 - 16.0;        

        return final;

    }
}
