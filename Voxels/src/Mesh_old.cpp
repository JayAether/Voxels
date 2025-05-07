#include "Mesh_old.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <iostream>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

#define POSITION_LOCATION  0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION    2


#define COLOR_TEXTURE_UNIT GL_TEXTURE0 // maybe move this to a more general file :P

bool Mesh::loadMesh(const std::string& filename)
{
    clear();

    glGenVertexArrays(1, &m_VAO); // problem looks to be with those two functions
    glBindVertexArray(m_VAO); // imma try another thingy (relating opengl ver 4.0)
    
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    
    //glCreateVertexArrays(1, &m_VAO);
    //glCreateBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    bool ret = false;
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(filename, ASSIMP_LOAD_FLAGS);

    if (pScene)
    {
        ret = initFromScene(pScene, filename);
    }
    else
    {
        printf("error parsing '%s': '%s'\n", filename.c_str(), importer.GetErrorString());
        //std::cout << "error parsing '%s': '%s'\n", filename.c_str(), importer.GetErrorString();
    }

    glBindVertexArray(0);

    return ret;
}


bool Mesh::initFromScene(const aiScene* pScene, const std::string& filename)
{
    m_Meshes.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);
    
    unsigned int numVertices = 0;
    unsigned int numIndices = 0;

    countVerticesAndIndices(pScene, numVertices, numIndices);

    reserveSpace(numVertices, numIndices);

    initAllMeshes(pScene);

    if (!initMaterials(pScene, filename))
    {
        return false;
    }

    populateBuffers();
    
    printf("%x\n", glGetError());
    return true;
}

void Mesh::countVerticesAndIndices(const aiScene* pScene, unsigned int& numVertices, unsigned int& numIndices)
{
    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        m_Meshes[i].materialIndex = pScene->mMeshes[i]->mMaterialIndex;
        m_Meshes[i].numIndices = pScene->mMeshes[i]->mNumFaces * 3; //m_Meshes[i].numIndices = pScene->mMeshes[i]->mFaces->mNumIndices; // test if this works as an alt later 
        m_Meshes[i].baseVertex = numVertices;
        m_Meshes[i].baseIndex = numIndices;

        numVertices += pScene->mMeshes[i]->mNumVertices;
        numIndices += m_Meshes[i].numIndices;
    }
}

void Mesh::reserveSpace(unsigned int numVertices, unsigned int numIndices)
{
    m_Positions.reserve(numVertices);
    m_Normals.reserve(numVertices);
    m_TexCoords.reserve(numVertices);
    m_Indices.reserve(numIndices);
}

void Mesh::initAllMeshes(const aiScene* pScene)
{
    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        initSingleMesh(paiMesh);
    }
}

void Mesh::initSingleMesh(const aiMesh* paiMesh)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // populate vertex atrribute vectors
    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
    {
        const aiVector3D& pPos      = paiMesh->mVertices[i];
        const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;
        const aiVector3D& pNormal   = paiMesh->mNormals[i];

        m_Positions.push_back(glm::vec3(pPos.x, pPos.y, pPos.z));
        m_TexCoords.push_back(glm::vec2(pTexCoord.x, pTexCoord.y));
        m_Normals.push_back(glm::vec3(pNormal.x, pNormal.y, pNormal.z));
    }

    // populate the index buffer
    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
    {
        const aiFace& face = paiMesh->mFaces[i];
        assert(face.mNumIndices == 3);
        m_Indices.push_back(face.mIndices[0]);
        m_Indices.push_back(face.mIndices[1]);
        m_Indices.push_back(face.mIndices[2]);

        // an alternative, should test if it works later
        /*for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            m_Indices.push_back(face.mIndices[j]);
        }*/
    }

}

bool Mesh::initMaterials(const aiScene* pScene, const std::string& filename)
{
    std::string::size_type slashIndex = filename.find_last_of("/");
    std::string dir;

    if (slashIndex == std::string::npos)
    {
        dir = ".";
    }
    else if (slashIndex == 0)
    {
        dir = "/";
    }
    else
    {
        dir = filename.substr(0, slashIndex);
    }

    bool ret = true;

    for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
    {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == aiReturn_SUCCESS) // if aiReturn_SUCCESS doesnt work, do AI_SUCCESS
            {
                std::string p(path.data);

                if (p.substr(0, 2) == ".\\")
                {
                    p = p.substr(2, p.size() - 2);
                }
                
                std::string fullPath = dir + "/" + p;

                m_Textures[i] = new Texture_old(GL_TEXTURE_2D, fullPath.c_str());

                if (!m_Textures[i]->load())
                {
                    printf("error loading texture '%s'\n", fullPath.c_str());
                    delete m_Textures[i];
                    ret = false;
                }
                else
                {
                    printf("loaded texture '%s'\n", fullPath.c_str());
                }
            
            }
        }
    }

    return ret;
}

void Mesh::populateBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), &m_Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);
}

void Mesh::render()
{
    glBindVertexArray(m_VAO);

    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        unsigned int materialIndex = m_Meshes[i].materialIndex;

        assert(materialIndex < m_Textures.size());

        if (m_Textures[materialIndex])
        {
            m_Textures[materialIndex]->bind(COLOR_TEXTURE_UNIT);
        }

        glDrawElementsBaseVertex(GL_TRIANGLES,
                                m_Meshes[i].numIndices,
                                GL_UNSIGNED_INT,
                                (void*)(sizeof(unsigned int) * m_Meshes[i].baseIndex),
                                m_Meshes[i].baseVertex);
    }

    glBindVertexArray(0);
}