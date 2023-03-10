#include "ResourceFactory.h"
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include "Game.h"

bool ResourceFactory::isInitialized = false;
Game* ResourceFactory::activeGame;
std::unordered_map<std::string, ResourceFactory::VertexShaderInfo> ResourceFactory::vShaders {};
std::unordered_map<std::string, ResourceFactory::PixelShaderInfo> ResourceFactory::pShaders {};
std::unordered_map<const wchar_t*, ResourceFactory::TextureInfo> ResourceFactory::textures {};

void ResourceFactory::Initialize(Game* game)
{
    if (isInitialized)
        return;

    activeGame = game;

    vShaders.insert({"base", {nullptr, nullptr}});
    pShaders.insert({"base", {nullptr, nullptr}});
    
    ID3DBlob* errorVertexCode = nullptr;
    auto res = D3DCompileFromFile(L"./Shaders/Base3d.hlsl",
        nullptr /*macros*/,
        nullptr /*include*/,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &(vShaders["base"].bc),
        &errorVertexCode);

    if (FAILED(res)) {
        // If the shader failed to compile it should have written something to the error message.
        if (errorVertexCode) {
            const char* compileErrors = static_cast<char*>(errorVertexCode->GetBufferPointer());

            std::cout << compileErrors << std::endl;
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else
        {
            MessageBox(game->Display->hWnd, L"Base3d.hlsl", L"Missing Shader File", MB_OK);
        }

        return;
    }

    ID3DBlob* errorPixelCode;
    res = D3DCompileFromFile(L"./Shaders/Base3d.hlsl",
        nullptr /*macros*/,
        nullptr /*include*/,
        "PSMain",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &(pShaders["base"].bc),
        &errorPixelCode);

    game->Device->CreateVertexShader(
        vShaders["base"].bc->GetBufferPointer(),
        vShaders["base"].bc->GetBufferSize(),
        nullptr, &(vShaders["base"].shader));

    game->Device->CreatePixelShader(
        pShaders["base"].bc->GetBufferPointer(),
        pShaders["base"].bc->GetBufferSize(),
        nullptr, &(pShaders["base"].shader));
}

ID3D11Resource* ResourceFactory::GetTextureBuffer(const wchar_t* name)
{
    if (textures.find(name) != textures.end())
        return textures[name].textureBuffer;

    auto res = DirectX::CreateDDSTextureFromFile(activeGame->Device.Get(), name, &textures[name].textureBuffer, &textures[name].textureView);

    activeGame->Context->GenerateMips(textures[name].textureView);

    return textures[name].textureBuffer;
}

ID3D11ShaderResourceView* ResourceFactory::GetTextureView(const wchar_t* name)
{
    if (textures.find(name) != textures.end())
        return textures[name].textureView;

    auto res = DirectX::CreateDDSTextureFromFile(activeGame->Device.Get(), name, &textures[name].textureBuffer, &textures[name].textureView);

    activeGame->Context->GenerateMips(textures[name].textureView);

    return textures[name].textureView;
}

ID3D11VertexShader* ResourceFactory::GetVertexShader(const std::string name)
{
    return vShaders[name].shader;
}

ID3D11PixelShader* ResourceFactory::GetPixelShader(const std::string name)
{
    return pShaders[name].shader;
}

ID3DBlob* ResourceFactory::GetVertexShaderBC(std::string name)
{
    return vShaders[name].bc;
}

ID3DBlob* ResourceFactory::GetPixelShaderBC(std::string name)
{
    return pShaders[name].bc;
}
