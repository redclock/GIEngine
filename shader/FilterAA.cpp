#include "FilterAA.h"
#include "GameEngine.h"

CFilterAA::CFilterAA()
{
    m_offsets2x2 = new float[2 * 4];
    m_offsets3x3 = new float[2 * 9];
    m_effect = LoadEffect("shaders/DS_Image.fx");
}

CFilterAA::~CFilterAA()
{
    delete[] m_offsets2x2;
    delete[] m_offsets3x3;
}

void CFilterAA::Init(UINT width, UINT height)
{
    m_width = width;
    m_height = height;
    float dx = 1.0f / width * 0.25f;
    float dy = 1.0f / height * 0.25f;

    m_offsets2x2[0] = dx;
    m_offsets2x2[1] = dy;
    m_offsets2x2[2] = dx;
    m_offsets2x2[3] = -dy;
    m_offsets2x2[4] = -dx;
    m_offsets2x2[5] = dy;
    m_offsets2x2[6] = -dx;
    m_offsets2x2[7] = -dy;

    dx = 1.0f / width * 0.5f / 3.0f;
    dy = 1.0f / height * 0.5f / 3.0f;

    m_offsets3x3[0] = -dx;
    m_offsets3x3[1] = -dy;
    m_offsets3x3[2] = 0;
    m_offsets3x3[3] = -dy;
    m_offsets3x3[4] = dx;
    m_offsets3x3[5] = -dy;

    m_offsets3x3[6] = -dx;
    m_offsets3x3[7] = 0;
    m_offsets3x3[8] = 0;
    m_offsets3x3[9] = 0;
    m_offsets3x3[10] = dx;
    m_offsets3x3[11] = 0;

    m_offsets3x3[12] = -dx;
    m_offsets3x3[13] = dy;
    m_offsets3x3[14] = 0;
    m_offsets3x3[15] = dy;
    m_offsets3x3[16] = dx;
    m_offsets3x3[17] = dy;


    m_targets[0].CreateTexture(GetDevice(), width, height, D3DFMT_A16B16G16R16F);
    m_targets[1].CreateTexture(GetDevice(), width, height, D3DFMT_A16B16G16R16F);
    m_targets[2].CreateTexture(GetDevice(), width, height, D3DFMT_A16B16G16R16F);
}

void CFilterAA::FilterImage(LPDIRECT3DTEXTURE9 texOrigin, int superRes)
{
    m_effect->SetTexture("g_texPosmap", texOrigin);

    if (superRes == 2)
    {
        m_effect->SetTechnique("Filter2x2");
        m_effect->SetFloatArray("g_Offset", m_offsets2x2, 2 * 4);
    }
    else
    {
        m_effect->SetTechnique("Filter3x3");
        m_effect->SetFloatArray("g_Offset", m_offsets3x3, 2 * 9);
    }


    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)m_width, (int)m_height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);
}

void CFilterAA::GenBuffers(CDeferredShade& defer, int superRes)
{
    m_effect->SetTexture("g_texPosmap", defer.GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer.GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", defer.GetTexture(CDeferredShade::MATERIAL));
    
    if (superRes == 2)
    {
        m_effect->SetTechnique("FilterBuffer2x2");
        m_effect->SetFloatArray("g_Offset", m_offsets2x2, 2 * 4);
    }
    else
    {
        m_effect->SetTechnique("FilterBuffer3x3");
        m_effect->SetFloatArray("g_Offset", m_offsets3x3, 2 * 9);
    }
    m_targets[0].BeginRenderTarget(0);
    m_targets[1].BeginRenderTarget(1);
    m_targets[2].BeginRenderTarget(2);

    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)m_width, (int)m_height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    m_targets[0].EndRenderTarget();
    m_targets[1].EndRenderTarget();
    m_targets[2].EndRenderTarget();
}

void CFilterAA::RotateLights(CRenderTarget& target, LPDIRECT3DTEXTURE9 texLights, 
                             int lightCount, int groupCount, D3DXMATRIX& matRot)
{
    m_effect->SetTexture("g_texPosmap", texLights);
    m_effect->SetMatrix("g_matRotate", &matRot);
    m_effect->SetTechnique("RotateLights");
    GetDevice()->BeginScene();
    target.BeginRenderTarget(0);

    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, lightCount, 2 * groupCount};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    target.EndRenderTarget();
    GetDevice()->EndScene();
}