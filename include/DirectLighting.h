#pragma once
#include "RenderTarget.h"

class Point3;
class CGameLight;
class CDeferredShade;
class CShadowMap;
class CFilterAA;

class CDirectLighting
{
private:
    LPD3DXEFFECT m_effect;

public:
    CDirectLighting();
    ~CDirectLighting();

    bool Render(UINT width, UINT height, const Point3& eyePos, 
        CGameLight* light, CDeferredShade* defer, CShadowMap* shadow, float ambient = 0);

    bool RenderWithEnvLight(UINT width, UINT height, const Point3& eyePos, 
        LPDIRECT3DTEXTURE9 front, LPDIRECT3DTEXTURE9 back, CDeferredShade* defer);

    bool RenderAA(UINT width, UINT height, const Point3& eyePos, 
        CGameLight* light, CDeferredShade* defer, float ambient = 0);

    bool RenderAATexture(UINT width, UINT height, const Point3& eyePos, 
        LPDIRECT3DTEXTURE9 texLight, int lightCount, float lightOffset,
        CDeferredShade* defer, float ambient = 0);


    bool RenderAAFilter(UINT width, UINT height, const Point3& eyePos, 
        CGameLight* light, CFilterAA* filter, float ambient = 0);

    bool RenderAAFilterTexture(UINT width, UINT height, const Point3& eyePos, 
        LPDIRECT3DTEXTURE9 texLight, int lightCount, float lightOffset,
        CFilterAA* filter, float ambient = 0);

};