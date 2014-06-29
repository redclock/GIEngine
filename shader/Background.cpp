#include "Background.h"
#include "GameEngine.h"
CBackground::CBackground()
{
    m_effect = LoadEffect("shaders/background.fx");
    m_color[0] = 0.0f;
    m_color[1] = 0.0f;
    m_color[2] = 0.0f;

    m_effect->SetFloatArray("g_Backcolor", m_color, 3);
    m_effect->SetTechnique("Backcolor");
}

CBackground::~CBackground()
{

}

void CBackground::Render(LPDIRECT3DTEXTURE9 diffuseTex, UINT width, UINT height)
{

    m_effect->SetTexture("g_texDiffusemap", diffuseTex);
    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);
}

void CBackground::SetBackColor(float r, float g, float b)
{
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;

    m_effect->SetFloatArray("g_Backcolor", m_color, 3);
    m_effect->SetTechnique("Backcolor");
}

D3DXMATRIX CBackground::ComputeTexToDirMatrix(const Point3& eyePos,  
                                              const D3DXMATRIX& matView, const D3DXMATRIX& matProj)
{
    // compute texture-coordination to direction transform
    D3DXMATRIX matTex
        (
        2, 0, 0, 0,
        0, -2, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1
        );

    float zn = - matProj._43 / matProj._33;

    D3DXMATRIX matInvProj
        (
        zn / matProj._11, 0, 0, 0,
        0, zn / matProj._22, 0, 0,
        0, 0, 0, 0,
        0, 0, zn, 1
        );

    D3DXMATRIX matTexToDir;
    D3DXMATRIX matInvView;
    D3DXMatrixInverse(&matInvView, NULL, &matView);
    D3DXMATRIX matPosToDir;
    D3DXMatrixTranslation(&matPosToDir, -eyePos.x, -eyePos.y, -eyePos.z);
    matTexToDir = matTex * matInvProj * matInvView * matPosToDir;
    m_effect->SetMatrix("g_mTexToDir", &matTexToDir);
    return matTexToDir;
}

void CBackground::SetEnvMap(LPDIRECT3DTEXTURE9 tex)
{
    m_effect->SetTexture("g_texEnvironment", tex);
    m_effect->SetTechnique("Envmap");
}