#include "DeferredShade.h"
#include "GameEngine.h"
#include <assert.h>

CDeferredShade::CDeferredShade()
{
    m_targets[0] = NULL;
    m_targets[1] = NULL;
    m_targets[2] = NULL;

    m_effect = LoadEffect("shaders/GBuffer.fx");
}

CDeferredShade::~CDeferredShade()
{
    Clear();
}

void CDeferredShade::SetSize( UINT width, UINT height )
{
    m_width = width;
    m_height = height;
}

bool CDeferredShade::CreateTarget(CDeferredShade::Target id, 
                                  D3DFORMAT format, D3DFORMAT depthFormat, 
                                  UINT levels, DWORD usage)
{
    assert(m_targets[id] == NULL);
    m_targets[id] = new CRenderTarget();
    return m_targets[id]->CreateTexture(GetDevice(), m_width, 
                                        m_height, format, 
                                        depthFormat, levels, usage);
}

bool CDeferredShade::BeginRender(bool viewSpace, const Point3& eyePos,
                                 CDeferredShade::Target target, 
                                 const D3DXMATRIX& mWorld,
                                 const D3DXMATRIX& mView, 
                                 const D3DXMATRIX& mProjection)
{
    m_matWVP = mWorld * mView * mProjection;
    m_matTransform = mWorld;
    if (viewSpace)
        m_matTransform *= mView;
    m_effect->SetMatrix("g_mWorldViewProjection", &m_matWVP);
    m_effect->SetMatrix("g_mTransform", &m_matTransform);
    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);

    BeginRenderTarget(target);

    GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
        0x00000000, 1.0f, 0); 

    if (FAILED(GetDevice()->BeginScene()))
        return false;
    UINT cPasses;
    if (FAILED(m_effect->Begin(&cPasses, 0)))
    {
        GetDevice()->EndScene();
        return false;
    }
    m_effect->BeginPass(0);
    return true;
}

bool CDeferredShade::EndRender()
{
    m_effect->EndPass();
    m_effect->End();
    GetDevice()->EndScene();
    
    EndRenderTarget();

    return true;
}

void CDeferredShade::Clear()
{
    delete m_targets[0];
    m_targets[0] = NULL;
    delete m_targets[1];
    m_targets[1] = NULL;
    delete m_targets[2];
    m_targets[2] = NULL;
}

void CDeferredShade::SetTexture(LPDIRECT3DTEXTURE9 texture)
{
    m_effect->SetTexture("g_Texture", texture);
    m_effect->CommitChanges();
}

void CDeferredShade::SetSufaceLevel(int level)
{
    for (int i = 0; i < 3; i++)
        if (m_targets[i])
            m_targets[i]->UpdateSurface(level);
}

void CDeferredShade::SetObjectTransform(const D3DXMATRIX& mat)
{
    D3DXMATRIXA16 mWVP = mat * m_matWVP;
    D3DXMATRIXA16 mTransform = mat * m_matTransform;

    m_effect->SetMatrix("g_mWorldViewProjection", &mWVP);
    m_effect->SetMatrix("g_mTransform", &mTransform);
    m_effect->CommitChanges();
}

void CDeferredShade::BeginRenderTarget(CDeferredShade::Target target)
{
    m_curTargets = target;

    switch (target)
    {
    case POSITION:
        m_effect->SetTechnique("GBufferPos");
        if (m_targets[POSITION])
            m_targets[POSITION]->BeginRenderTarget(0);
        break;
    case NORMAL:
        m_effect->SetTechnique("GBufferNorm");
        if (m_targets[NORMAL])
            m_targets[NORMAL]->BeginRenderTarget(0);
        break;
    case MATERIAL:
        m_effect->SetTechnique("GBufferMat");
        if (m_targets[MATERIAL])
            m_targets[MATERIAL]->BeginRenderTarget(0);
        break;
    case ALL:
        m_effect->SetTechnique("GBufferAll");
        if (m_targets[POSITION])
        {
            m_targets[POSITION]->BeginRenderTarget(0);

        }
        if (m_targets[NORMAL])
            m_targets[NORMAL]->BeginRenderTarget(1);
        if (m_targets[MATERIAL])
            m_targets[MATERIAL]->BeginRenderTarget(2);
        break;
    case FOR_TRACE:
        m_effect->SetTechnique("GBufferTrace");
        if (m_targets[NORMAL])
            m_targets[NORMAL]->BeginRenderTarget(0);
        if (m_targets[MATERIAL])
            m_targets[MATERIAL]->BeginRenderTarget(1);

        break;
    default:
        return;
    }
}

void CDeferredShade::EndRenderTarget()
{
    switch (m_curTargets)
    {
    case POSITION:
        if (m_targets[POSITION])
            m_targets[POSITION]->EndRenderTarget();
        break;
    case NORMAL:
        if (m_targets[NORMAL])
            m_targets[NORMAL]->EndRenderTarget();
        break;
    case MATERIAL:
        if (m_targets[MATERIAL])
            m_targets[MATERIAL]->EndRenderTarget();
        break;
    case ALL:
        if (m_targets[POSITION])
            m_targets[POSITION]->EndRenderTarget();
        if (m_targets[NORMAL])
            m_targets[NORMAL]->EndRenderTarget();
        if (m_targets[MATERIAL])
            m_targets[MATERIAL]->EndRenderTarget();
        break;
    case FOR_TRACE:
        if (m_targets[NORMAL])
            m_targets[NORMAL]->EndRenderTarget();
        if (m_targets[MATERIAL])
            m_targets[MATERIAL]->EndRenderTarget();
    default:
        return;
    }
}