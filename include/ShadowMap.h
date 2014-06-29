#pragma once
#include "RenderTarget.h"

class CGameLight;

class CShadowMap
{
private:
    UINT m_width;
    UINT m_height;
    CRenderTarget* m_target;
    LPD3DXEFFECT m_effect;
    D3DXMATRIX m_lightMat;
    D3DXMATRIX m_textureMat;

public:
    CShadowMap();
    ~CShadowMap();

    void SetSize(UINT width, UINT height);
    bool CreateTarget(D3DFORMAT format, 
        D3DFORMAT depthFormat = D3DFMT_UNKNOWN, UINT levels = 1,
        DWORD usage = NULL);
    CRenderTarget* GetRenderTarget() { return m_target; };
    LPDIRECT3DTEXTURE9 GetTexture() { return m_target->GetTexture(); };
    const D3DXMATRIX& GetTextureMatrix() const { return m_textureMat; };
    bool BeginRender(CGameLight* light);
    bool EndRender();

    void ReleaseTarget();
    void SetObjectTransform(const D3DXMATRIX& mat);
};