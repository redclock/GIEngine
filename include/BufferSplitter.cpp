#include "BufferSplitter.h"
#include "GameEngine.h"



CBufferSplitter::CBufferSplitter()
{
    m_effect = LoadEffect("shaders/BufferSplitter.fx");
}

CBufferSplitter::~CBufferSplitter()
{

}

void CBufferSplitter::Init(UINT width, UINT height, UINT gridw, UINT gridh)
{
    m_width = width;
    m_height = height;
    m_gridw = gridw;
    m_gridh = gridh;
    
    if(m_targetIndex.GetTexture() == NULL)
        m_targetIndex.CreateTexture(GetDevice(), m_width, m_height, D3DFMT_G16R16);

    int screen[2] = {width, height};
    int grid[2] = {gridw, gridh};
    m_effect->SetIntArray("g_ScreenSize", screen, 2);
    m_effect->SetIntArray("g_GridSize", grid, 2);

    m_effect->SetTechnique("CreateIndex");
    
    if (SUCCEEDED(GetDevice()->BeginScene()))
    {
        m_targetIndex.BeginRenderTarget(0);

        DX_BEGIN_EFFECT(m_effect);

        RECT rt = {0, 0, (int)width, (int)height};
        Get2D()->DrawRect(&rt, 0x00FFFFFF);

        DX_END_EFFECT(m_effect);
        m_targetIndex.EndRenderTarget();
        GetDevice()->EndScene();
    }

    m_effect->SetTexture("g_texIndex", m_targetIndex.GetTexture());
}

void CBufferSplitter::SplitTexture(LPDIRECT3DTEXTURE9 tex)
{
    m_effect->SetTechnique("SplitTexture");

    m_effect->SetTexture("g_texSource", tex);
    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)m_width, (int)m_height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);
}

void CBufferSplitter::SplitGBuffer(LPDIRECT3DTEXTURE9 tex1, LPDIRECT3DTEXTURE9 tex2, 
                                   LPDIRECT3DTEXTURE9 tex3)
{
    m_effect->SetTechnique("SplitTexture3");

    m_effect->SetTexture("g_texSource", tex1);
    m_effect->SetTexture("g_texSource1", tex2);
    m_effect->SetTexture("g_texSource2", tex3);
    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)m_width, (int)m_height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

}

void CBufferSplitter::SplitTextureInvert( LPDIRECT3DTEXTURE9 tex )
{
    m_effect->SetTechnique("SplitTextureInvert");

    m_effect->SetTexture("g_texSource", tex);
    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)m_width, (int)m_height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

}