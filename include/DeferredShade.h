#pragma once
#include "RenderTarget.h"
#include "Point3.h"

class CDeferredShade
{
public:
    enum Target
    {
        POSITION = 0,
        NORMAL = 1,
        MATERIAL = 2,
        ALL,
        FOR_TRACE
    };

private:
    UINT m_width;
    UINT m_height;
    CRenderTarget* m_targets[3];
    LPD3DXEFFECT m_effect;
    Target m_curTargets;
    D3DXMATRIX m_matWVP;
    D3DXMATRIX m_matTransform;

public:
    CDeferredShade();
    ~CDeferredShade();
    void SetSize(UINT width, UINT height);
    bool CreateTarget(Target id, D3DFORMAT format, 
        D3DFORMAT depthFormat = D3DFMT_UNKNOWN, UINT levels = 1,
        DWORD usage = NULL);
    CRenderTarget* GetRenderTarget(Target id) { return m_targets[id]; };
    LPDIRECT3DTEXTURE9 GetTexture(Target id) { return m_targets[id]->GetTexture(); };

    bool BeginRender(bool viewSpace, const Point3& eyePos, 
        Target target, const D3DXMATRIX& mWorld,
        const D3DXMATRIX& mView, const D3DXMATRIX& mProjection);
    bool EndRender();

    void BeginRenderTarget(Target target);
    void EndRenderTarget();

    void Clear();

    void SetTexture(LPDIRECT3DTEXTURE9 texture);
    void SetObjectTransform(const D3DXMATRIX& mat);
    void SetSufaceLevel(int level);

    UINT GetWidth() const { return m_width; }
    UINT GetHeight() const { return m_height; }

    const D3DXMATRIX& GetWVPMatrix() const { return m_matWVP; }
    void SetWVPMatrix(const D3DXMATRIX& mat) { m_matWVP = mat; }
};