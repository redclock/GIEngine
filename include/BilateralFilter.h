#pragma once
#include <d3dx9.h>
#include "RenderTarget.h"

class CBilateralFilter
{
private:
    UINT m_width;
    UINT m_height;
    LPD3DXEFFECT m_effect;

    float m_sceneScale;
    float m_sharpness;
    float m_blurRadius;
    float m_edgeThreshold;
    float m_offset[2];
    CRenderTarget m_targetTemp;

private:
    void UpdateBlur();
    void UpdateSharpness();
    void SetSceneScale(float scale);

public:
    CBilateralFilter();
    ~CBilateralFilter();
    
    bool Init(UINT width, UINT height);
    void Filter(LPDIRECT3DTEXTURE9 texSource, LPDIRECT3DTEXTURE9 texDepth, 
        LPDIRECT3DTEXTURE9 texNormal, float radius, float sharpness);
};