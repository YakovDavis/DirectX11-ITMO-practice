#include "MeshRenderComponent.h"
#include <iostream>

using namespace DirectX;
using namespace SimpleMath;

void MeshRenderComponent::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }
    for (UINT i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void MeshRenderComponent::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex point;

        point.pos.x = mesh->mVertices[i].x;
        point.pos.y = mesh->mVertices[i].y;
        point.pos.z = mesh->mVertices[i].z;
        point.pos.w = 1.0f;

        if (mesh->mTextureCoords[0])
        {
            point.tex.x = mesh->mTextureCoords[0][i].x;
            point.tex.y = mesh->mTextureCoords[0][i].y;
        }

        point.normal.x = mesh->mNormals[i].x;
        point.normal.y = mesh->mNormals[i].y;
        point.normal.z = mesh->mNormals[i].z;
        point.normal.w = 0.0f;

        points.push_back(point);
    }

    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (UINT j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
}

MeshRenderComponent::MeshRenderComponent(Game* g, const std::string fileNameModel, const wchar_t* fileNameTexture) :
    BaseRenderComponent(g), fNameModel(fileNameModel), isPickedUp(false)
{
    textureFileName = fileNameTexture;
}

void MeshRenderComponent::Initialize()
{
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(fNameModel.c_str(), aiProcess_Triangulate|aiProcess_ConvertToLeftHanded);

    ProcessNode(pScene->mRootNode, pScene);
    
    BaseRenderComponent::Initialize();
}
