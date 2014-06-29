#include <cassert>
#include "RenderTarget.h"
#include "MyLib.h"
#include "GameEngine.h"

CRenderTarget::CRenderTarget()
{
    m_texture = NULL;
    m_surface = NULL;
    m_dsurface = NULL;

    m_oldSurface = NULL;
    m_oldDsurface = NULL;
}

CRenderTarget::~CRenderTarget()
{
    ReleaseTexture();
}

bool CRenderTarget::CreateTexture(LPDIRECT3DDEVICE9 device, UINT width, UINT height, 
                                  D3DFORMAT format, D3DFORMAT depthFormat, UINT levels, DWORD usage)
{
    assert(m_texture == NULL && m_surface == NULL && m_dsurface == NULL);

    m_width = width;
    m_height = height;
    m_format = format;
    m_dformat = depthFormat;
    m_device = device;

    usage |= D3DUSAGE_RENDERTARGET;
    //if (levels == 0)
    //    usage |= D3DUSAGE_AUTOGENMIPMAP;
    
    if (FAILED(device->CreateTexture(width, height, levels, 
               usage, format, 
               D3DPOOL_DEFAULT, &m_texture, NULL)))
    {
        MessageBox(NULL, "Create Texture Error", "ERROR", 0);
        return false;
    }

    if (FAILED(m_texture->GetSurfaceLevel(0, &m_surface)))
    {
        m_texture->Release();
        m_texture = NULL;
        MessageBox(NULL, "Get Surface Error", "ERROR", 0);
        return false;
    }

    CreateDepthSurface(depthFormat);
    InitViewPort();

    m_level = 0;

    return true;
}

void CRenderTarget::CreateDepthSurface(D3DFORMAT format)
{
    if (format != D3DFMT_UNKNOWN)
    {
        // Create a depth stencil surface
        if(FAILED(m_device->CreateDepthStencilSurface(
            m_width, m_height, format, D3DMULTISAMPLE_NONE, 0, TRUE, &m_dsurface, NULL)))
        {
            MessageBox(NULL, "Create Depth Surface Error", "ERROR", 0);
            return;
        }
    }
}

void CRenderTarget::InitViewPort()
{
    // Create a new viewport
    //
    m_viewport.X = 0;
    m_viewport.Y = 0;
    m_viewport.Width  = m_width;
    m_viewport.Height = m_height;
    m_viewport.MinZ = 0.0f;
    m_viewport.MaxZ = 1.0f;
}

void CRenderTarget::ReleaseTexture()
{
    if (m_oldDsurface)
    {
        m_oldDsurface->Release();
        m_oldDsurface = NULL;
    }
    if (m_oldSurface)
    {
        m_oldSurface->Release();
        m_oldSurface = NULL;
    }
    if (m_dsurface)
    {
        m_dsurface->Release();
        m_dsurface = NULL;
    }
    if (m_surface)
    {
        m_surface->Release();
        m_surface = NULL;
    }
    if (m_texture)
    {
        m_texture->Release();
        m_texture = NULL;
    }
}

bool CRenderTarget::BeginRenderTarget(DWORD index)
{
    if (m_surface == NULL)
    {
        MessageBox(NULL, "Render target is not available", "ERROR", 0);
        return false;
    }

    m_index = index;
    if (index == 0)
    {
        m_device->GetViewport(&m_oldViewport);
        m_device->GetRenderTarget(index, &m_oldSurface);
        if (m_dsurface != NULL)
            m_device->GetDepthStencilSurface(&m_oldDsurface);
    }
    else
    {
        m_device->GetRenderTarget(index, &m_oldSurface);
    }

    if (FAILED(m_device->SetViewport(&m_viewport)))
        return false;
    if (FAILED(m_device->SetRenderTarget(index, m_surface)))
        return false;
    if (m_dsurface != NULL)
        if (FAILED(m_device->SetDepthStencilSurface(m_dsurface)))
            return false;
    
    return true;
}

bool CRenderTarget::EndRenderTarget()
{
    if (m_index == 0)
    {
        if (FAILED(m_device->SetViewport(&m_oldViewport)))
            return false;
        if (FAILED(m_device->SetRenderTarget(m_index, m_oldSurface)))
            return false;
        if (m_dsurface != NULL)
            if (FAILED(m_device->SetDepthStencilSurface(m_oldDsurface)))
                return false;
        if (m_oldDsurface != NULL)
        {
            m_oldDsurface->Release();
            m_oldDsurface = NULL;
        }
        if (m_oldSurface != NULL)
        {
            m_oldSurface->Release();
            m_oldSurface = NULL;
        }
    }
    else
    {
        if (FAILED(m_device->SetRenderTarget(m_index, m_oldSurface)))
            return false;
        if (m_oldSurface != NULL)
        {
            m_oldSurface->Release();
            m_oldSurface = NULL;
        }
    }
    return true;
}

bool CRenderTarget::ApplyDepthBuffer()
{
    if (m_dsurface == NULL || m_oldDsurface != NULL)
        return false;
    
    m_device->GetDepthStencilSurface(&m_oldDsurface);
    if (FAILED(m_device->SetDepthStencilSurface(m_dsurface)))
    {
        printf("fail to set depth buffer\n");
        return false;
    }
    return true;
}

bool CRenderTarget::RestoreDepthBuffer()
{
    if (m_oldDsurface == NULL)
        return false;
    if (FAILED(m_device->SetDepthStencilSurface(m_oldDsurface)))
        return false;
    m_oldDsurface->Release();
    m_oldDsurface = NULL;
    return true;
}

bool CRenderTarget::UpdateSurface(int level)
{
    assert (m_surface && m_texture);
    m_surface->Release();
    m_surface = NULL;
    if (FAILED(m_texture->GetSurfaceLevel(level, &m_surface)))
    {
        MessageBox(NULL, "Get Surface Error", "ERROR", 0);
        return false;
    }  
    m_level = level;
    m_viewport.X = 0;
    m_viewport.Y = 0;
    m_viewport.Width  = m_width / (1 << level);
    m_viewport.Height = m_height / (1 << level);

    return true;
}

void CRenderTarget::Fill(CGame2D* g2d, int x, int y, int w, int h, float color[4])
{
    LPD3DXEFFECT effect = LoadEffect("utils.fx");
    if (effect == NULL) return;
    BeginRenderTarget(0);
    if (SUCCEEDED(GetDevice()->BeginScene()))
    {
        CGame2D* g2d = Get2D();
        effect->SetFloatArray("g_Fill", color, 4);
        effect->SetTechnique("FillTexture");
        DX_BEGIN_EFFECT(effect);
        g2d->DrawRect(x, y, w, h, 0xFFFFFFFF);
        DX_END_EFFECT(effect);
        GetDevice()->EndScene();
    }
    EndRenderTarget();
}

void CRenderTarget::Fill(float r, float g, float b, float a)
{
    float color[4] = {r, g, b, a};
    Fill(Get2D(), 0, 0, m_viewport.Width, m_viewport.Height, color);
}

