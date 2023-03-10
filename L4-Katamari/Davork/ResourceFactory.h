#pragma once
#include <unordered_map>
#include "DDSTextureLoader.h"

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
private:
    static bool isInitialized;
    static Game* activeGame;
    static std::unordered_map<std::string, VertexShaderInfo> vShaders;
    static std::unordered_map<std::string, PixelShaderInfo> pShaders;
    static std::unordered_map<const wchar_t*, TextureInfo> textures;
public:
    ResourceFactory() = delete;
    static void Initialize(Game* game);
    static ID3D11Resource* GetTextureBuffer(const wchar_t* name);
    static ID3D11ShaderResourceView* GetTextureView(const wchar_t* name);
    static ID3D11VertexShader* GetVertexShader(std::string name);
    static ID3D11PixelShader* GetPixelShader(std::string name);
    static ID3DBlob* GetVertexShaderBC(std::string name);
    static ID3DBlob* GetPixelShaderBC(std::string name);
};
