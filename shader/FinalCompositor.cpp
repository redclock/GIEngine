#include "FinalCompositor.h"
#include "GameEngine.h"

CFinalCompositor::CFinalCompositor()
{
    m_effect = LoadEffect("shaders/Final.fx");
}

CFinalCompositor::~CFinalCompositor()
{

}

void CFinalCompositor::RenderFinal(float gamma, UINT width, UINT height)
{
    GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
        D3DCOLOR_RGBA(0, 1, 0, 1), 1.0f, 0); 

    CGame2D* g2d = Get2D();

    m_effect->SetFloat("g_Gamma", gamma);

    DX_BEGIN_EFFECT(m_effect);
    RECT rt = {0, 0, (int)width, (int)height};
    g2d->DrawRect(&rt, 0x00FFFFFF);
    DX_END_EFFECT(m_effect);
}

void CFinalCompositor::SetTextures(LPDIRECT3DTEXTURE9 tex1, float factor1, 
                                   LPDIRECT3DTEXTURE9 tex2, float factor2, 
                                   LPDIRECT3DTEXTURE9 tex3, float factor3,
                                   LPDIRECT3DTEXTURE9 tex4, float factor4)
{
    float factors[4] = {factor1, factor2, factor3, factor4};
    m_effect->SetFloatArray("g_Factors", factors, 4);
    m_effect->SetTexture("g_texture1", tex1);
    m_effect->SetTexture("g_texture2", tex2);
    m_effect->SetTexture("g_texture3", tex3);
    m_effect->SetTexture("g_texture4", tex4);
}