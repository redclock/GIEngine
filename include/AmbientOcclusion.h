#pragma once

#include <d3dx9.h>
#include "Point3.h"
#include "DynamicTexture.h"

class CDeferredShade;

class CAmbientOcclusion
{
private:
    LPD3DXEFFECT m_effect;
    CDynamicTexture m_texRandom;
    CDynamicTexture m_texSSRandom;
    UINT m_samples;
    UINT m_patternCount;
    void CreateRandomTexture(UINT samples, UINT count);

public:
    CAmbientOcclusion();
    ~CAmbientOcclusion();
    
    void SetEnvMap(LPDIRECT3DTEXTURE9 tex);

    void RenderAO(UINT width, UINT height, CDeferredShade& defer, 
        const Point3& eyepos, float radius);
    void RenderDirect(UINT width, UINT height, CDeferredShade& defer, 
                      const Point3& eyepos, LPDIRECT3DTEXTURE9 texLight, int lightCount, 
                      float lightOffsetU, float lightOffsetV);
    void SetLightRotMat(const D3DXMATRIX& mat);
    void RenderDiffuse(UINT width, UINT height, CDeferredShade& defer, 
                       LPDIRECT3DTEXTURE9 texDiffuse);
    void RenderDirectSplit(UINT width, UINT height, UINT gridw, UINT gridh,
        CDeferredShade& defer, LPDIRECT3DTEXTURE9 texPos, const Point3& eyepos, float radius, 
        LPDIRECT3DTEXTURE9 texLight, int lightCount, 
        float lightOffsetU, float lightOffsetV);

};