#pragma once
#include <d3dx9.h>
#include "Point3.h"

class CEnvViewPoint
{
public:
    Point3 pos;
    bool used;
    bool predefined;
    D3DXMATRIX matView[6];
    CEnvViewPoint(const Point3& p);
    CEnvViewPoint();

    void SetPosition(const Point3& p);
};