#include "DirectionalLight.h"

using namespace DirectX;
using namespace SimpleMath;

DirectionalLight::DirectionalLight() : lightDirection_(0.0f, 1.0f, 0.0f, 0.0f),
    lightColor_(1.0f, 1.0f, 1.0f, 1.0f)
{
}

void DirectionalLight::FillCb(PerSceneCb& cb) const
{
    cb.LightPos = GetDirection();
    cb.LightColor = GetColor();
}

Matrix DirectionalLight::GetLightViewProj(std::vector<Vector4> corners) const
{
    auto center = Vector3::Zero;
    for (const auto& v : corners)
    {
        center += static_cast<Vector3>(v);
    }
    center /= static_cast<float>(corners.size());
    
    const auto lightView = Matrix::CreateLookAt(center + lightDirection_, center, Vector3::Up);
    
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = Vector4::Transform(v, lightView);
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }
    
    constexpr float zMult = 10.0f; // how much geometry to include from outside the view frustum
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }
   
    const auto lightProjection = Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);

    return lightView * lightProjection;
}
