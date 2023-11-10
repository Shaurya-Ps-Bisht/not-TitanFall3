#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;


#include "Mesh.h"
#include "Shader.h"

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



class Model
{
public:
    // model data 
    vector<TextureStruct> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    bool gammaCorrection;


    Model(const char* path);
    void Draw(Shader& shader);

private:
    // model data
    vector<Mesh> meshes;
    string directory;
    glm::vec3 m_position, m_axis, m_scale;
    float  m_angle;

private:
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransformation);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<TextureStruct> loadMaterialTextures(aiMaterial* mat, aiTextureType type,string typeName);
    unsigned int TextureFromFile(const char* path, const string& directory);
    void SetTransform(glm::vec3 pos, glm::vec3 axisRotation, float angle, glm::vec3 scale);
    glm::mat4 AiMatrix4x4ToGlm(const aiMatrix4x4* from);
};
