#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"

#include <string>
#include <vector>
#include <iostream>

#define INVALID_MATERIAL 0xFFFFFFFF




class Mesh
{
public:
    Mesh() {};

    ~Mesh() 
    {
        for (unsigned int i = 0; i < m_Textures.size(); i++)
        {
            delete m_Textures[i];
        }
    };

    bool loadMesh(const std::string& filename);

    void render();
    
    glm::mat4& getWorldTransform() { // this one is fucked, it needs to return the m_worldTransform var // rn it just returns an identity mat always
        glm::mat4 m_mat = glm::mat4(1.0f); 
        return m_mat;
    }

private:
	void clear() {};
    void reserveSpace(unsigned int numVertices, unsigned int numIndices);




    struct MeshEntry
    {
        MeshEntry()
        {
            numIndices = 0;
            baseVertex = 0;
            baseIndex = 0;
            materialIndex = INVALID_MATERIAL;
        }

        unsigned int numIndices;
        unsigned int baseVertex;
        unsigned int baseIndex;
        unsigned int materialIndex;
    };

    std::vector<MeshEntry> m_Meshes;
    std::vector<Texture*> m_Textures;

    enum BUFFER_TYPE
    {
        INDEX_BUFFER,
        POS_VB,
        TEXCOORD_VB,
        NORMAL_VB,
        WVP_MAT_VB, // required only for instancing (not sure what this is)
        WORLD_MAT_VB, // required only for instancing (not sure what this is)
        NUM_BUFFERS
    };

    GLuint m_VAO;

    GLuint m_Buffers[NUM_BUFFERS] = { 0 };

    glm::mat4 m_worldTransform = glm::mat4(1.0f);
    
private:

    bool initFromScene(const aiScene* pScene, const std::string& filename);
    void countVerticesAndIndices(const aiScene* pScene, unsigned int& numVertices, unsigned int& numIndices);
    void initAllMeshes(const aiScene* pScene);
    void initSingleMesh(const aiMesh* paiMesh);
    bool initMaterials(const aiScene* pScene, const std::string& filename);
    void populateBuffers();

    // temporary holding space until we can throw it into the glDraw executive
    std::vector<glm::vec3> m_Positions;
    std::vector<glm::vec2> m_TexCoords;
    std::vector<glm::vec3> m_Normals;
    std::vector<unsigned int> m_Indices;
};

