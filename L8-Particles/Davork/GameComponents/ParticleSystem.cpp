#include "ParticleSystem.h"

#include "BaseRenderComponent.h"
#include "../Game.h"
#include "../ResourceFactory.h"

using namespace DirectX;
using namespace SimpleMath;

static D3D_SHADER_MACRO* GetMacros(const ParticleSystem::ComputeFlags& flag)
{
    auto ShaderMacros = new D3D_SHADER_MACRO[] { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

    int macrosCount = 0;
    if (static_cast<bool>(flag & ParticleSystem::ComputeFlags::INJECTION))
    {
        ShaderMacros[macrosCount++] = { "INJECTION", "1" };
    }
    
    if (static_cast<bool>(flag & ParticleSystem::ComputeFlags::SIMULATION))
    {
        ShaderMacros[macrosCount++] = { "SIMULATION", "1" };
    }

    if (static_cast<bool>(flag & ParticleSystem::ComputeFlags::ADD_GRAVITY))
    {
        ShaderMacros[macrosCount++] = { "ADD_GRAVITY", "1" };
    }

    return ShaderMacros;
}

static float RandomFloatInRange(float low, float high)
{
    return (high - low) * static_cast<float>(rand()) / RAND_MAX + low;
}

static Vector4 RandomVectorInRange(Vector3 range)
{
    Vector4 res = Vector4::Zero;
    res.x = RandomFloatInRange(-range.x, range.x);
    res.y = RandomFloatInRange(-range.y, range.y);
    res.z = RandomFloatInRange(-range.z, range.z);
    return res;
}

ParticleSystem::ParticleSystem(Game* g) : GameComponent(g)
{
    Width = 1.0f;
    Height = 1.0f;
    Length = 1.0f;
    Position = DirectX::SimpleMath::Vector3(0, 2, 0);

    GravityAffected = true;

    EmitterSettings.Enabled = true;
    EmitterSettings.ParticlesPerSecond = 100.0f;
    EmitterSettings.Color0 = Vector4::One;
    EmitterSettings.Velocity = Vector3::Zero;
    EmitterSettings.NudgeColorLum = true;
    EmitterSettings.NudgeColorHue = false;
    EmitterSettings.Size0 = 0.05f;
    EmitterSettings.Size1 = 0.02f;
    EmitterSettings.LifeTime = 1.0f;
    EmitterSettings.NudgeLifeTime = false;

    TextureFileName = L"Textures/smoke.dds";

    GroundLevel = -5;
}

void ParticleSystem::GetGroupSize(int particleCount, int& groupSizeX, int& groupSizeY)
{
    int numGroups = (particleCount % 256 != 0) ? ((particleCount / 256) + 1) : (particleCount / 256);
    auto root = std::sqrt(static_cast<double>(numGroups));
    root = std::ceil(root);
    groupSizeX = static_cast<int>(root);
    groupSizeY = groupSizeX;
}

void ParticleSystem::Initialize()
{
    LoadShaders();
    CreateBuffers();

    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = true;
    rastDesc.FillMode = D3D11_FILL_SOLID;

    game->GetDevice()->CreateRasterizerState(&rastDesc, rastState.GetAddressOf());

    CD3D11_BLEND_DESC blendDesc = {};
    blendDesc.IndependentBlendEnable = false;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

    game->GetDevice()->CreateBlendState(&blendDesc, blendState.GetAddressOf());

    CD3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.StencilEnable = false;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.StencilReadMask = 0x00;
    depthStencilDesc.StencilWriteMask = 0x00;

    game->GetDevice()->CreateDepthStencilState(&depthStencilDesc, depthState.GetAddressOf());

    D3D11_SAMPLER_DESC samplerStateDesc = {};
    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.MinLOD = 0.0f;
    samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

    game->GetDevice()->CreateSamplerState(&samplerStateDesc, samplerState.GetAddressOf());
}

void ParticleSystem::Update()
{
    if (EmitterSettings.Enabled)
    {
        emitterTimeSave += game->DeltaTime();
	    while (emitterTimeSave / (1.0f / EmitterSettings.ParticlesPerSecond) >= 1)
	    {
            Particle p = {};
            p.LifeTime = EmitterSettings.LifeTime + (EmitterSettings.NudgeLifeTime ? RandomFloatInRange(-0.5f, 0.5f) : 0.0f);
            p.Velocity = Vector4(EmitterSettings.Velocity.x, EmitterSettings.Velocity.y, EmitterSettings.Velocity.z, 0.0f) + (EmitterSettings.NudgeVelocity ? RandomVectorInRange(Vector3(0.05f, 0.05f, 0.05f)) : Vector4::Zero);
            p.Color0 = (EmitterSettings.Color0 + (EmitterSettings.NudgeColorHue ? RandomVectorInRange(Vector3(0.05f, 0.05f, 0.05f)) : Vector4::Zero)) * (EmitterSettings.NudgeColorLum ? RandomFloatInRange(0.9f, 1.1f) : 1.0f);
            p.Size0Size1 = Vector2(EmitterSettings.Size0, EmitterSettings.Size1);
            p.Position = Vector4(Length * RandomFloatInRange(-1.0f, 1.0f), Height * RandomFloatInRange(-1.0f, 1.0f), Width * RandomFloatInRange(-1.0f, 1.0f), 0.0f);
            p.Position.Normalize();
            p.Position.x *= RandomFloatInRange(0.0, Width / 2.0f);
            p.Position.y *= RandomFloatInRange(0.0, Height / 2.0f);
            p.Position.z *= RandomFloatInRange(0.0, Length / 2.0f);
            p.Position.w = 1.0f;
            p.MaxLifeTime = p.LifeTime;
            AddParticle(p);
            emitterTimeSave -= 1.0f / EmitterSettings.ParticlesPerSecond;
	    }
    }

    int groupSizeX, groupSizeY;
    GetGroupSize(ParticlesCount, groupSizeX, groupSizeY);

    constData.World = Matrix::CreateTranslation(Position);
    constData.View = game->GetCamera()->GetView();
    constData.Proj = game->GetCamera()->GetProj();
    constData.DeltaTimeMaxParticlesGroupdimGround = Vector4(game->DeltaTime(), ParticlesCount, groupSizeY, GroundLevel);

    game->GetContext()->UpdateSubresource(constBuf.Get(), 0, nullptr, &constData, 0, 0);
    game->GetContext()->CSSetConstantBuffers(0, 1, constBuf.GetAddressOf());

    constexpr UINT counterKeepValue = -1;
    constexpr UINT counterZero = 0;

    game->GetContext()->CSSetUnorderedAccessViews(0, 1, uavSrc.GetAddressOf(), &counterKeepValue);
    game->GetContext()->CSSetUnorderedAccessViews(1, 1, uavDst.GetAddressOf(), &counterZero);

    if (GravityAffected)
    {
        game->GetContext()->CSSetShader(ComputeShaders[ComputeFlags::SIMULATION | ComputeFlags::ADD_GRAVITY].Get(), nullptr, 0);
    }
    else
    {
        game->GetContext()->CSSetShader(ComputeShaders[ComputeFlags::SIMULATION].Get(), nullptr, 0);
    }

    if (groupSizeX > 0)
    {
        game->GetContext()->Dispatch(groupSizeX, groupSizeY, 1);
    }

    if (InjectionCount > 0)
    {
        int injSizeX, injSizeY;
        GetGroupSize(InjectionCount, injSizeX, injSizeY);

        constData.DeltaTimeMaxParticlesGroupdimGround = Vector4(game->DeltaTime(), static_cast<float>(InjectionCount), static_cast<float>(injSizeY), GroundLevel);

        game->GetContext()->UpdateSubresource(constBuf.Get(), 0, nullptr, &constData, 0, 0);
        game->GetContext()->CSSetConstantBuffers(0, 1, constBuf.GetAddressOf());

        game->GetContext()->UpdateSubresource(injectionBuf.Get(), 0, nullptr, injectionParticles, 0, 0);

        game->GetContext()->CSSetUnorderedAccessViews(0, 1, injUav.GetAddressOf(), &InjectionCount);
        game->GetContext()->CSSetShader(ComputeShaders[ComputeFlags::INJECTION].Get(), nullptr, 0);

        game->GetContext()->Dispatch(injSizeX, injSizeY, 1);

        InjectionCount = 0;
    }

    ID3D11UnorderedAccessView* nuPtr = nullptr;
    game->GetContext()->CSSetUnorderedAccessViews(0, 1, &nuPtr, &counterZero);
    game->GetContext()->CSSetUnorderedAccessViews(1, 1, &nuPtr, &counterZero);

    game->GetContext()->CopyStructureCount(countBuf.Get(), 0, uavDst.Get());

    D3D11_MAPPED_SUBRESOURCE sr;
    game->GetContext()->Map(countBuf.Get(), 0, D3D11_MAP_READ, 0, &sr);
    UINT* data = reinterpret_cast<UINT*>(sr.pData);
    ParticlesCount = data[0];
    game->GetContext()->Unmap(countBuf.Get(), 0);

    SwapBuffers();
}

void ParticleSystem::Draw()
{
    game->GetContext()->ClearState();

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> oldRastState = nullptr;
    game->GetContext()->RSGetState(oldRastState.GetAddressOf());
    game->GetContext()->RSSetState(rastState.Get());

    Microsoft::WRL::ComPtr<ID3D11BlendState> oldBlendState = nullptr;
    UINT oldBlendMask = 0;
    float oldBlendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    game->GetContext()->OMGetBlendState(oldBlendState.GetAddressOf(), oldBlendFactor, &oldBlendMask);

    float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    game->GetContext()->OMSetBlendState(blendState.Get(), blendFactor, 0xffffffff);

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> oldDepthState = nullptr;
    UINT oldStencilRef = 0;
    game->GetContext()->OMGetDepthStencilState(oldDepthState.GetAddressOf(), &oldStencilRef);
    game->GetContext()->OMSetDepthStencilState(depthState.Get(), 0);
    game->GetContext()->OMSetRenderTargets(1, game->GetMainRTV(), game->GetMainDSV());

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<float>(game->GetDisplay()->ClientWidth);
    viewport.Height = static_cast<float>(game->GetDisplay()->ClientHeight);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    game->GetContext()->RSSetViewports(1, &viewport);

    game->GetContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    game->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    game->GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
    game->GetContext()->VSSetShaderResources(0, 1, srvSrc.GetAddressOf());
    game->GetContext()->VSSetConstantBuffers(0, 1, constBuf.GetAddressOf());
    if (IsTextured)
    {
        game->GetContext()->PSSetShader(pixelShaderTex.Get(), nullptr, 0);
        const auto texture = ResourceFactory::GetTextureView(TextureFileName);
        game->GetContext()->PSSetShaderResources(0, 1, &texture);
        game->GetContext()->PSSetSamplers(0, 1, samplerState.GetAddressOf());
    }
    else
    {
        game->GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
    }

    game->GetContext()->DrawIndexedInstancedIndirect(bufArgs.Get(), 0);

    game->GetContext()->ClearState();

    game->GetContext()->RSSetState(oldRastState.Get());
    game->GetContext()->OMSetBlendState(oldBlendState.Get(), oldBlendFactor, oldBlendMask);
    game->GetContext()->OMSetDepthStencilState(oldDepthState.Get(), oldStencilRef);
}

void ParticleSystem::DestroyResources()
{
}

void ParticleSystem::LoadShaders()
{
    Microsoft::WRL::ComPtr<ID3DBlob> vertexBC;
    Microsoft::WRL::ComPtr<ID3DBlob> pixelBC;
    Microsoft::WRL::ComPtr<ID3DBlob> errorCode;

    auto res = D3DCompileFromFile(L"./Shaders/Particles.hlsl",
        nullptr,
        nullptr,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        vertexBC.GetAddressOf(),
        errorCode.GetAddressOf());

    res = D3DCompileFromFile(L"./Shaders/Particles.hlsl",
        nullptr,
        nullptr,
        "PSMain",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        pixelBC.GetAddressOf(),
        errorCode.GetAddressOf());

    game->GetDevice()->CreateVertexShader(vertexBC.Get()->GetBufferPointer(),
        vertexBC.Get()->GetBufferSize(), nullptr, vertexShader.GetAddressOf());

    game->GetDevice()->CreatePixelShader(pixelBC.Get()->GetBufferPointer(),
        pixelBC.Get()->GetBufferSize(), nullptr, pixelShader.GetAddressOf());

    Microsoft::WRL::ComPtr<ID3DBlob> pixelTexBC;

    res = D3DCompileFromFile(L"./Shaders/ParticlesTex.hlsl",
        nullptr,
        nullptr,
        "PSMain",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        pixelTexBC.GetAddressOf(),
        errorCode.GetAddressOf());

    game->GetDevice()->CreatePixelShader(pixelTexBC.Get()->GetBufferPointer(),
        pixelTexBC.Get()->GetBufferSize(), nullptr, pixelShaderTex.GetAddressOf());
    
    std::vector<ComputeFlags> flags = {
        ComputeFlags::INJECTION,
        ComputeFlags::SIMULATION,
        ComputeFlags::SIMULATION | ComputeFlags::ADD_GRAVITY,
    };

    for (auto& flag : flags)
    {
        D3D_SHADER_MACRO* macros = GetMacros(flag);
        Microsoft::WRL::ComPtr<ID3DBlob> computeBC;
        res = D3DCompileFromFile(L"./Shaders/ParticlesSim.hlsl",
            macros,
            nullptr,
            "CSMain",
            "cs_5_0",
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
            0,
            computeBC.GetAddressOf(),
            errorCode.GetAddressOf());

        Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
        game->GetDevice()->CreateComputeShader(computeBC.Get()->GetBufferPointer(),
            computeBC.Get()->GetBufferSize(), nullptr, computeShader.GetAddressOf());

        ComputeShaders.emplace(flag, computeShader);
    }
}

void ParticleSystem::CreateBuffers()
{
    D3D11_BUFFER_DESC constBufDesc = {};
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.Usage = D3D11_USAGE_DEFAULT;
    constBufDesc.MiscFlags = 0;
    constBufDesc.CPUAccessFlags = 0;
    constBufDesc.ByteWidth = sizeof(ConstData);

    game->GetDevice()->CreateBuffer(&constBufDesc, nullptr, constBuf.GetAddressOf());

    D3D11_BUFFER_DESC particleBufDesc = {};
    particleBufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    particleBufDesc.Usage = D3D11_USAGE_DEFAULT;
    particleBufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    particleBufDesc.CPUAccessFlags = 0;
    particleBufDesc.StructureByteStride = sizeof(Particle);
    particleBufDesc.ByteWidth = MaxParticlesCount * sizeof(Particle);


    game->GetDevice()->CreateBuffer(&particleBufDesc, nullptr, bufFirst.GetAddressOf());
    game->GetDevice()->CreateBuffer(&particleBufDesc, nullptr, bufSecond.GetAddressOf());

    game->GetDevice()->CreateShaderResourceView(bufFirst.Get(), nullptr, srvFirst.GetAddressOf());
    game->GetDevice()->CreateShaderResourceView(bufSecond.Get(), nullptr, srvSecond.GetAddressOf());

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer = D3D11_BUFFER_UAV{
        0,
        MaxParticlesCount,
        D3D11_BUFFER_UAV_FLAG_APPEND
    };

    game->GetDevice()->CreateUnorderedAccessView(bufFirst.Get(), &uavDesc, uavFirst.GetAddressOf());
    game->GetDevice()->CreateUnorderedAccessView(bufSecond.Get(), &uavDesc, uavSecond.GetAddressOf());

    srvSrc = srvFirst;
    uavSrc = uavFirst;
    srvDst = srvSecond;
    uavDst = uavSecond;

    ID3D11UnorderedAccessView* nuPtr = nullptr;
    game->GetContext()->CSSetUnorderedAccessViews(0, 1, uavSrc.GetAddressOf(), &MaxParticlesCount);
    game->GetContext()->CSSetUnorderedAccessViews(0, 1, &nuPtr, nullptr);

    D3D11_BUFFER_DESC countBufDesc = {};
    countBufDesc.BindFlags = 0;
    countBufDesc.Usage = D3D11_USAGE_STAGING;
    countBufDesc.MiscFlags = 0;
    countBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    countBufDesc.StructureByteStride = 0;
    countBufDesc.ByteWidth = 4;

    game->GetDevice()->CreateBuffer(&countBufDesc, nullptr, countBuf.GetAddressOf());

    D3D11_BUFFER_DESC injectionBufDesc = {};
    injectionBufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    injectionBufDesc.Usage = D3D11_USAGE_DEFAULT;
    injectionBufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    injectionBufDesc.CPUAccessFlags = 0;
    injectionBufDesc.StructureByteStride = sizeof(Particle);
    injectionBufDesc.ByteWidth = MaxParticlesInjectionCount * sizeof(Particle);

    game->GetDevice()->CreateBuffer(&injectionBufDesc, nullptr, injectionBuf.GetAddressOf());

    D3D11_UNORDERED_ACCESS_VIEW_DESC injUavDesc = {};
    injUavDesc.Format = DXGI_FORMAT_UNKNOWN;
    injUavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    injUavDesc.Buffer = D3D11_BUFFER_UAV{
        0,
        MaxParticlesInjectionCount,
        D3D11_BUFFER_UAV_FLAG_APPEND
    };

    game->GetDevice()->CreateUnorderedAccessView(injectionBuf.Get(), &injUavDesc, injUav.GetAddressOf());

    D3D11_BUFFER_DESC indexBufDesc;
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(UINT) * MaxParticlesCount * 6;

    D3D11_SUBRESOURCE_DATA indexData;
    UINT* indices = new UINT[MaxParticlesCount * 6];
    for (UINT i = 0; i < MaxParticlesCount; ++i)
    {
        indices[i * 6 + 0] = i * 4 + 0;
        indices[i * 6 + 1] = i * 4 + 1;
        indices[i * 6 + 2] = i * 4 + 2;

        indices[i * 6 + 3] = i * 4 + 1;
        indices[i * 6 + 4] = i * 4 + 0;
        indices[i * 6 + 5] = i * 4 + 3;
    }
    indexData.pSysMem = &indices[0];
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    game->GetDevice()->CreateBuffer(&indexBufDesc, &indexData, indexBuffer.GetAddressOf());

    delete[] indices;

    D3D11_SUBRESOURCE_DATA argsData;
    DrawArgs args = {};
    args.IndexCountPerInstance = MaxParticlesCount * 6;
    args.InstanceCount = 1;
    argsData.pSysMem = &args;
    argsData.SysMemPitch = 0;
    argsData.SysMemSlicePitch = 0;

    D3D11_BUFFER_DESC argsBufDesc = {};
    argsBufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    argsBufDesc.Usage = D3D11_USAGE_DEFAULT;
    argsBufDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    argsBufDesc.CPUAccessFlags = 0;
    argsBufDesc.StructureByteStride = sizeof(DrawArgs);
    argsBufDesc.ByteWidth = sizeof(DrawArgs);

    auto res = game->GetDevice()->CreateBuffer(&argsBufDesc, &argsData, bufArgs.GetAddressOf());
}

void ParticleSystem::AddParticle(const Particle& p)
{
    injectionParticles[InjectionCount++] = p;
}

void ParticleSystem::SwapBuffers()
{
    auto tmpSrv = srvSrc;
    auto tmpUav = uavSrc;

    srvSrc = srvDst;
    uavSrc = uavDst;

    srvDst = tmpSrv;
    uavDst = tmpUav;
}
