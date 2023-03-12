#pragma once
#include <unordered_map>
#include "Vertex.h"
#include "DDSTextureLoader.h"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

class Game;

class ResourceFactory
{
public:
    struct TextureInfo
    {
        ID3D11Resource* textureBuffer;
        ID3D11ShaderResourceView* textureView;
    };
    struct VertexShaderInfo
    {
        ID3D11VertexShader* shader;
        ID3DBlob* bc;
    };
    struct PixelShaderInfo
    {
        ID3D11PixelShader* shader;
        ID3DBlob* bc;
    };
    struct GeometryInfo
    {
        std::vector<Vertex> points {};
        std::vector<UINT> indices {};
    };
private:
    static bool isInitialized;
    static Game* activeGame;
    static std::unordered_map<std::string, VertexShaderInfo> vShaders;
    static std::unordered_map<std::string, PixelShaderInfo> pShaders;
    static std::unordered_map<const wchar_t*, TextureInfo> textures;
    static std::unordered_map<std::string, GeometryInfo> meshes;
    static void LoadTexture(const wchar_t* name);
    static void LoadMesh(std::string name);
    static void ProcessNode(const std::string name, aiNode* node, const aiScene* scene);
    static void ProcessMesh(const std::string name, aiMesh* mesh, const aiScene* scene);
public:
    ResourceFactory() = delete;
    static void Initialize(Game* game);
    static ID3D11Resource* GetTextureBuffer(const wchar_t* name);
    static ID3D11ShaderResourceView* GetTextureView(const wchar_t* name);
    static ID3D11VertexShader* GetVertexShader(std::string name);
    static ID3D11PixelShader* GetPixelShader(std::string name);
    static ID3DBlob* GetVertexShaderBC(std::string name);
    static ID3DBlob* GetPixelShaderBC(std::string name);
    static const std::vector<Vertex>& GetPoints(std::string name);
    static const std::vector<UINT>& GetIndices(std::string name);
};
