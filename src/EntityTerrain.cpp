#include "EntityTerrain.h"

EntityTerrain::EntityTerrain(const std::string &name, const unsigned char *data, const int &width, const int &height)
    : Entity(name, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), m_shader), m_ResolutionWidth(width),
      m_ResolutionHeight(height)
    
{
    GLint maxTessLevel;
    
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    std::cout << "Max available tess level: " << maxTessLevel << std::endl;

    // build and compile our shader program
    // ------------------------------------
    m_shader = Shader("../../res/Shaders/Terrain/terrain.vs",
        "../../res/Shaders/Terrain/terrain.fs", nullptr,
        "../../res/Shaders/Terrain/terrain.tcs",
        "../../res/Shaders/Terrain/terrain.tes");


    m_shader.setInt("heightMap", 0);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);        

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

            vertices.push_back(-width / 2.0f + width * i / (float)rez);         // v.x
            vertices.push_back(0.0f);                                           // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
            vertices.push_back(i / (float)rez);                                 // u
            vertices.push_back((j + 1) / (float)rez);                           // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
            vertices.push_back((i + 1) / (float)rez); // u
            vertices.push_back(j / (float)rez); // v         

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

EntityTerrain::~EntityTerrain()
{
}

void EntityTerrain::draw(const float &deltaTime, Camera &cam, bool instanced, float elapsedTime, lightDir dLight,
                         std::vector<lightPoint> &lightPoints, glm::mat4 lightSpaceMatrix)

{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    int widthUniformLocation = glGetUniformLocation(m_shader.m_ID, "ourColor");
    m_shader.use();
    m_shader.setInt("uHeightMap", 0);
    m_shader.setInt("sandTex", 1);
    m_shader.setInt("shadowMap", 2);

    m_shader.setFloat("farPlane", cam.m_farPlane);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_sand);
    m_shader.setVec2("uTexelSize", (float)1.0 / m_ResolutionWidth, (float)1.0 / m_ResolutionHeight);

    // view/projection transformations
    m_shader.setMat4("projection", cam.GetProjectionMatrix());
    m_shader.setMat4("view", cam.GetViewMatrix());

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    m_shader.setMat4("model", model);

    //Lighting setup
    m_shader.setVec3("viewPos", cam.m_cameraPos);
    m_shader.setVec3("dirLight.direction", dLight.m_direction);
    m_shader.setVec3("dirLight.color", dLight.m_color);

    // render the 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glBindVertexArray(m_terrainVAO);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);

    glDisable(GL_CULL_FACE);
}

void EntityTerrain::drawDirLight(const float &deltaTime, bool instanced, Camera &cam, float elapsedTime,
                                 lightDir dLight, Shader &shader)
{
    int widthUniformLocation = glGetUniformLocation(m_shader.m_ID, "ourColor");
    shader.use();
    shader.setInt("uHeightMap", 0);
    shader.setInt("sandTex", 1);
    shader.setInt("shadowMap", 2);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_sand);
    m_shader.setVec2("uTexelSize", (float)1.0 / m_ResolutionWidth, (float)1.0 / m_ResolutionHeight);

    // view/projection transformations

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    m_shader.setMat4("model", model);

    // render the 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glBindVertexArray(m_terrainVAO);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
}

void EntityTerrain::LoadFromFile(const char* filename)
{
    
}



void EntityTerrain::loadSand()
{
    int width, height, bpp;
    unsigned char * m_LocalBuffer = stbi_load("../../res/textures/sand/sand1.jpg", &width, &height, &bpp, 4);

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

