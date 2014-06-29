#pragma once
#include <d3dx9.h>
#include "RenderTarget.h"
#include "DeferredShade.h"

class CFilterAA
{
private:
    float m_filter;
    LPD3DXEFFECT m_effect;
    CRenderTarget m_targets[3];
    
    UINT m_width;
    UINT m_height;

    float* m_offsets2x2;
    float* m_offsets3x3;
public:
    CFilterAA();
    ~CFilterAA();

    void Init(UINT width, UINT height);
    LPDIRECT3DTEXTURE9 GetTexture(int index) const { return m_targets[index].GetTexture(); }

    void FilterImage(LPDIRECT3DTEXTURE9 texOrigin, int superRes);
    void GenBuffers(CDeferredShade& defer, int superRes);
    void RotateLights(CRenderTarget& target, LPDIRECT3DTEXTURE9 texLights, 
        int lightCount, int groupCount, D3DXMATRIX& matRot);
};