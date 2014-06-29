#include "EnvViewPoint.h"
#include "MyLib.h"

CEnvViewPoint::CEnvViewPoint(const Point3& p)
{
    SetPosition(p);
    used = false;
}

CEnvViewPoint::CEnvViewPoint()
{
    used = false;
}

void CEnvViewPoint::SetPosition(const Point3& p)
{
    pos = p;
    for (int i = 0; i < 6; i++)
    {
        matView[i] = GetCubeMapViewMatrix(p.x, p.y, p.z, i);
    }
}
