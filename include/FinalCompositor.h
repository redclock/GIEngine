#pragma once
#include <d3dx9.h>
#include <vector>

class CFinalCompositor
{
private:
    LPD3DXEFFECT m_effect;

public:
    CFinalCompositor();
    ~CFinalCompositor();

    void SetTextures(LPDIRECT3DTEXTURE9 tex1, float factor1,
        LPDIRECT3DTEXTURE9 tex2 = NULL, float factor2 = 0,
        LPDIRECT3DTEXTURE9 tex3 = NULL, float factor3 = 0,
        LPDIRECT3DTEXTURE9 tex4 = NULL, float factor4 = 0);
    void RenderFinal(float gamma, UINT width, UINT height);
};