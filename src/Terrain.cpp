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
    loadSand();
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

void Terrain::Draw(glm::mat4 projection , glm::mat4 view, lightDir dLight, glm::vec3 viewPos)
{
    int widthUniformLocation = glGetUniformLocation(m_terrainShader.m_ID, "ourColor");
    m_terrainShader.use();
    m_terrainShader.setInt("uHeightMap", 0);
    m_terrainShader.setInt("sandTex", 1);
    m_terrainShader.setInt("shadowMap", 2);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_sand);
    m_terrainShader.setVec2("uTexelSize", 1 / m_ResolutionWidth, 1 / m_ResolutionHeight);

    // view/projection transformations
    m_terrainShader.setMat4("projection", projection);
    m_terrainShader.setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    m_terrainShader.setMat4("model", model);

    //Lighting setup
    m_terrainShader.setVec3("viewPos", viewPos);
    m_terrainShader.setVec3("dirLight.direction", dLight.m_direction);
    m_terrainShader.setVec3("dirLight.color", dLight.m_color);

    // render the 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glBindVertexArray(m_terrainVAO);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
}

void Terrain::DrawDepth(Shader& shader)
{
    int widthUniformLocation = glGetUniformLocation(m_terrainShader.m_ID, "ourColor");
    shader.use();
    shader.setInt("uHeightMap", 0);
    shader.setInt("sandTex", 1);
    shader.setInt("shadowMap", 2);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_sand);
    m_terrainShader.setVec2("uTexelSize", 1 / m_ResolutionWidth, 1 / m_ResolutionHeight);

    // view/projection transformations

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

        unsigned char* offset00 = data + index00;
        unsigned char* offset10 = data + index10;
        unsigned char* offset01 = data + index01;
        unsigned char* offset11 = data + index11;

        // Bilinear interpolation
        float h00 = ((float)offset00[0] / 255.0f) * 100.0f - 16.0f;
        float h10 = ((float)offset10[0] / 255.0f) * 100.0f - 16.0f;
        float h01 = ((float)offset01[0] / 255.0f) * 100.0f - 16.0f;
        float h11 = ((float)offset11[0] / 255.0f) * 100.0f - 16.0f;

        float finalHeight = (1.0f - dz) * ((1.0f - dx) * h00 + dx * h10) +
            dz * ((1.0f - dx) * h01 + dx * h11);

        return finalHeight;
    }
    else
    {
        return 0.0f;
    }
}

void Terrain::loadSand()
{
    int width, height, bpp;
    unsigned char * m_LocalBuffer = stbi_load("res/textures/sand/sand1.jpg", &width, &height, &bpp, 4);

    glGenTextures(1, &m_sand);
    glBindTexture(GL_TEXTURE_2D, m_sand);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (m_LocalBuffer) {
        stbi_image_free(m_LocalBuffer);
    }
}

