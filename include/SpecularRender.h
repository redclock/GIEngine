#pragma once
#include "RenderTarget.h"

class Point3;
class CDeferredShade;
class CCubeMap;
class CGameLight;

class CSpecularRender
{
private:
    LPD3DXEFFECT m_effect;

public:
    CSpecularRender();
    ~CSpecularRender();
    void SetSpecularProperty(const Point3& reflect, const Point3& refract, float index);
    bool Render(UINT width, UINT height, const Point3& eyePos, 
        const Point3& center,
        CDeferredShade* defer, CCubeMap* cube, CGameLight * light);

};