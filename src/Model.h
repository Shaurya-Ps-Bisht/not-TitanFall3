#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


#include "Mesh.h"
#include "Shader.h"

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;

    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;

};


class Model
{
public:
    // model data 
    vector<TextureStruct> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    bool gammaCorrection;


    Model(const char* path);
    void Draw(Shader& shader);

public:
    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

private:
    // model data
    vector<Mesh> meshes;
    string directory;
    glm::vec3 m_position, m_axis, m_scale;
    float  m_angle;

    std::map<string, BoneInfo> m_BoneInfoMap; 
    int m_BoneCounter = 0;
    
    void SetVertexBoneDataToDefault(Vertex& vertex);
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

private:
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransformation, bool a);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<TextureStruct> loadMaterialTextures(aiMaterial* mat, aiTextureType type,string typeName);
    unsigned int TextureFromFile(const char* path, const string& directory);
    void SetTransform(glm::vec3 pos, glm::vec3 axisRotation, float angle, glm::vec3 scale);
};

