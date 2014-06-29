#pragma once
#include <d3dx9.h>
#include "RenderTarget.h"

class CBufferSplitter
{
private:
    LPD3DXEFFECT m_effect;
    UINT m_width;
    UINT m_height;
    UINT m_gridw;
    UINT m_gridh;
    CRenderTarget m_targetIndex;

public:
    CBufferSplitter();
    ~CBufferSplitter();

    void Init(UINT width, UINT height, UINT gridw, UINT gridh);
    LPDIRECT3DTEXTURE9 GetIndexBuffer() const { return m_targetIndex.GetTexture(); }
    void SplitTexture(LPDIRECT3DTEXTURE9 tex);
    void SplitGBuffer(LPDIRECT3DTEXTURE9 tex1, LPDIRECT3DTEXTURE9 tex2, 
        LPDIRECT3DTEXTURE9 tex3);
    void SplitTextureInvert(LPDIRECT3DTEXTURE9 tex);

};