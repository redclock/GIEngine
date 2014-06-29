#pragma once
#include "RenderTarget.h"

class CGameLight;

class CTransShadowMap
{
private:
    LPD3DXEFFECT m_effect;
    UINT m_width;
    UINT m_height;
    CRenderTarget m_target;
    D3DXMATRIX m_textureMat;
    D3DXMATRIX m_lightMat;

public:
    CTransShadowMap();
    ~CTransShadowMap();

    void Init(UINT width, UINT height, D3DFORMAT format, 
        D3DFORMAT depthFormat = D3DFMT_UNKNOWN, UINT levels = 1,
        DWORD usage = NULL);

    LPDIRECT3DTEXTURE9 GetTexture() { return m_target.GetTexture(); };
    const D3DXMATRIX& GetTextureMatrix() const { return m_textureMat; };
    bool BeginRender(CGameLight* light);
    bool EndRender();

    void SetObjectTransform(const D3DXMATRIX& mat);
};