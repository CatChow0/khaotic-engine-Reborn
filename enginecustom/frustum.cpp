#include "frustum.h"

void Frustum::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
    XMMATRIX matrix;
    XMVECTOR planes[6];

    // Calculate the minimum Z distance in the frustum.
    float zMinimum = -projectionMatrix.r[3].m128_f32[2] / projectionMatrix.r[2].m128_f32[2];
    float r = screenDepth / (screenDepth - zMinimum);
    projectionMatrix.r[2].m128_f32[2] = r;
    projectionMatrix.r[3].m128_f32[2] = -r * zMinimum;

    // Create the frustum matrix from the view matrix and updated projection matrix.
    matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

    // Calculate near plane of frustum.
    planes[0] = XMPlaneNormalize(XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[2],
        matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[2],
        matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[2],
        matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[2]));

    // Calculate far plane of frustum.
    planes[1] = XMPlaneNormalize(XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[2],
        matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[2],
        matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[2],
        matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[2]));

    // Calculate left plane of frustum.
    planes[2] = XMPlaneNormalize(XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[0],
        matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[0],
        matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[0],
        matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[0]));

    // Calculate right plane of frustum.
    planes[3] = XMPlaneNormalize(XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[0],
        matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[0],
        matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[0],
        matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[0]));

    // Calculate top plane of frustum.
    planes[4] = XMPlaneNormalize(XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[1],
        matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[1],
        matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[1],
        matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[1]));

    // Calculate bottom plane of frustum.
    planes[5] = XMPlaneNormalize(XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[1],
        matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[1],
        matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[1],
        matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[1]));

    for (int i = 0; i < 6; i++)
    {
        m_planes[i] = planes[i];
    }
}

bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius, float tolerance)
{
    // Vérifiez chaque plan du frustum pour voir si le cube est à l'intérieur
    for (int i = 0; i < 6; i++)
    {
        XMVECTOR plane = m_planes[i];
        if (XMVectorGetX(plane) * (xCenter - radius) + XMVectorGetY(plane) * (yCenter - radius) + XMVectorGetZ(plane) * (zCenter - radius) + XMVectorGetW(plane) > -tolerance)
            continue;
        if (XMVectorGetX(plane) * (xCenter + radius) + XMVectorGetY(plane) * (yCenter - radius) + XMVectorGetZ(plane) * (zCenter - radius) + XMVectorGetW(plane) > -tolerance)
            continue;
        if (XMVectorGetX(plane) * (xCenter - radius) + XMVectorGetY(plane) * (yCenter + radius) + XMVectorGetZ(plane) * (zCenter - radius) + XMVectorGetW(plane) > -tolerance)
            continue;
        if (XMVectorGetX(plane) * (xCenter + radius) + XMVectorGetY(plane) * (yCenter + radius) + XMVectorGetZ(plane) * (zCenter - radius) + XMVectorGetW(plane) > -tolerance)
            continue;
        if (XMVectorGetX(plane) * (xCenter - radius) + XMVectorGetY(plane) * (yCenter - radius) + XMVectorGetZ(plane) * (zCenter + radius) + XMVectorGetW(plane) > -tolerance)
            continue;
        if (XMVectorGetX(plane) * (xCenter + radius) + XMVectorGetY(plane) * (yCenter - radius) + XMVectorGetZ(plane) * (zCenter + radius) + XMVectorGetW(plane) > -tolerance)
            continue;
        if (XMVectorGetX(plane) * (xCenter - radius) + XMVectorGetY(plane) * (yCenter + radius) + XMVectorGetZ(plane) * (zCenter + radius) + XMVectorGetW(plane) > -tolerance)
            continue;
        if (XMVectorGetX(plane) * (xCenter + radius) + XMVectorGetY(plane) * (yCenter + radius) + XMVectorGetZ(plane) * (zCenter + radius) + XMVectorGetW(plane) > -tolerance)
            continue;

        // Si le cube est en dehors de l'un des plans, il n'est pas dans le frustum
        return false;
    }

    // Si le cube est à l'intérieur de tous les plans, il est dans le frustum
    return true;
}
