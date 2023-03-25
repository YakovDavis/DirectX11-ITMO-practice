﻿#include "Camera.h"
#include "SimpleMath.h"

using namespace DirectX;
using namespace SimpleMath;

Camera::Camera()
{
    viewProj = Matrix::Identity;
    
    FOV = XM_PI / 4.0f;
    AspectRatio = 1.0f;
    NearPlane = 1.0f;
    FarPlane = 1000.f;
    Position = Vector3::Backward * 10.0f;
    Target = Vector3::Zero;
    Up = Vector3::Up;
    IsOrthographic = false;
    OrthographicWidth = 50.0f;
    OrthographicHeight = 50.0f;
}

void Camera::UpdateMatrix()
{
    if (IsOrthographic)
        viewProj = Matrix::CreateLookAt(Position, Target, Up) * Matrix::CreateOrthographic(OrthographicWidth, OrthographicHeight, NearPlane / 10.0f, FarPlane);
    else
        viewProj = Matrix::CreateLookAt(Position, Target, Up) * Matrix::CreatePerspectiveFieldOfView(FOV, AspectRatio, NearPlane, FarPlane);
}

Matrix Camera::GetMatrix() const
{
    return viewProj;
}

std::vector<Vector4> Camera::GetFrustumCornersWorldSpace() const
{
    const auto inv = viewProj.Invert();
    
    std::vector<Vector4> frustumCorners;
    frustumCorners.reserve(8);
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const Vector4 pt = Vector4::Transform(Vector4(2.0f * x - 1.0f, 2.0f * y - 1.0f, z, 1.0f), inv);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }
    
    return frustumCorners;
}
