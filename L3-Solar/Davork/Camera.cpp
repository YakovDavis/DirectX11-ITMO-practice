#include "Camera.h"
#include "SimpleMath.h"

using namespace DirectX;
using namespace SimpleMath;

Camera::Camera()
{
    viewProj = Matrix::Identity;
    
    FOV = 0.25f * XM_PI;
    AspectRatio = 1.0f;
    NearPlane = 1.0f;
    FarPlane = 1000.f;
    Position = Vector3::Backward * 5;
    Target = Vector3::Zero;
    Up = Vector3::Up;
}

void Camera::UpdateMatrix()
{
    viewProj = Matrix::CreateLookAt(Position, Target, Up) * Matrix::CreatePerspectiveFieldOfView(FOV, AspectRatio, NearPlane, FarPlane);
}

Matrix Camera::GetMatrix() const
{
    return viewProj;
}
