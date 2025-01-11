#include <DirectXMath.h>
using namespace DirectX;

class Frustum
{
public:
    void ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix);
    bool CheckCube(float xCenter, float yCenter, float zCenter, float radius, float tolerance);

private:
    XMVECTOR m_planes[6];
};