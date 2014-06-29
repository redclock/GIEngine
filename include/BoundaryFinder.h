#pragma once
#include <d3dx9.h>
#include "Point3.h"
#include "RenderTarget.h"

class CDeferredShade;

class CBoundaryFinder
{
private:
    UINT m_width;
    UINT m_height;
    LPD3DXEFFECT m_effect;
    CRenderTarget m_targetSwap[2];
    void RenderBoundary(UINT width, UINT height);

public:
    CBoundaryFinder();
    ~CBoundaryFinder();
    void Init(UINT width, UINT height);
    int GetIndex(int level) const { return 1 - level % 2; };
    void OnelevelRenderBoundary(int level, const Point3& eyePos, CDeferredShade* defer);
    void MultilevelRenderBoundary(int startLevel, const Point3& eyePos, CDeferredShade* defer);
    LPDIRECT3DTEXTURE9 GetTexture(int index) { return m_targetSwap[index].GetTexture(); }
};