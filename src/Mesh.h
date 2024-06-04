#pragma once

#ifndef MESH_H
#define MESH_H

#define MAX_BONE_INFLUENCE 4

#include <glad.h> // holds all OpenGL type declarations

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <unordered_map>

#include "Shader.h"
// #include "ShadowManager.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct VertexStruct
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct TextureStruct
{
    unsigned int id;
    string type;
    string path;
};

class Mesh
{
  public:
    // mesh Data
    vector<VertexStruct> vertices;
    vector<unsigned int> indices;
    vector<TextureStruct> textures;
    unsigned int VAO;
    unsigned int instanceAmount;

    // constructor
    Mesh(vector<VertexStruct> vertices, vector<unsigned int> indices, vector<TextureStruct> textures);

    // render the mesh
    void Draw(Shader &shader);
    void DrawInstanced(Shader &shader);
    void SetTransformationMatrix(const glm::mat4 &transformationMatrix);

  private:
    void setupInstanceMatrix();

  private:
    // render data
    unsigned int VBO, EBO;
    std::unordered_map<std::string, unsigned int> textureNrs = {
        {"texture_diffuse", 1},  {"texture_specular", 1}, {"texture_normal", 1},    {"texture_height", 1},
        {"texture_emission", 1}, {"texture_metallic", 1}, {"texture_roughness", 1}, {"texture_ao", 1}};

    // initializes all the buffer objects/arrays
    void setupMesh();
};
#endif
