#include <cassert>
#include "DynamicTexture.h"

CDynamicTexture::CDynamicTexture()
{
    m_textureDefault = NULL;
    m_surfaceDefault = NULL;
    m_textureSystem = NULL;
    m_surfaceSystem = NULL;
    m_device = NULL;
}

CDynamicTexture::~CDynamicTexture()
{
    ReleaseTextures();
}

bool CDynamicTexture::CreateTexture(LPDIRECT3DDEVICE9 device, UINT width, UINT height, D3DFORMAT format)
{
    assert(m_textureSystem == NULL && m_surfaceSystem == NULL
        && m_textureDefault == NULL && m_surfaceDefault == NULL);
    m_device = device;

    m_width = width;
    m_height = height;
    m_format = format;

    if (CreateDefaultTexture() == false)
        return false;

    if (FAILED(device->CreateTexture(width, height, 1, 
        D3DUSAGE_DYNAMIC, format, 
        D3DPOOL_SYSTEMMEM, &m_textureSystem, NULL)))
    {
        MessageBox(NULL, "Create Texture Error", "ERROR", 0);
        return false;
    }
    if (FAILED(m_textureSystem->GetSurfaceLevel(0, &m_surfaceSystem)))
    {
        m_textureSystem->Release();
        m_textureSystem = NULL;
        MessageBox(NULL, "Get Surface Error", "ERROR", 0);
        return false;
    } 
    return true;
}

void CDynamicTexture::ReleaseTextures()
{
    ReleaseDefaultTexture();
    if (m_textureSystem)
    {
        m_textureSystem->Release();
        m_textureSystem = NULL;
    }
    if (m_surfaceSystem)
    {
        m_surfaceSystem->Release();
        m_surfaceSystem = NULL;
    }
}


void CDynamicTexture::ReleaseDefaultTexture()
{
    if (m_textureDefault)
    {
        m_textureDefault->Release();
        m_textureDefault = NULL;
    }
    if (m_surfaceDefault)
    {
        m_surfaceDefault->Release();
        m_surfaceDefault = NULL;
    }
}

bool CDynamicTexture::LockRect(D3DLOCKED_RECT *pLockedRect, const RECT* rect, DWORD flags)
{
    if (FAILED(m_surfaceSystem->LockRect(pLockedRect, rect, flags)))
    {
        MessageBox(NULL, "Lock rect Error", "ERROR", 0);
        return false;
    }
    return true;
}

bool CDynamicTexture::UnlockRect()
{
    return (bool) SUCCEEDED(m_surfaceSystem->UnlockRect());
}

bool CDynamicTexture::Update(const RECT* src, const POINT* dst)
{
    return (bool) SUCCEEDED(m_device->UpdateSurface(m_surfaceSystem, src, m_surfaceDefault, dst));
}

void CDynamicTexture::OnLostDevice()
{
    ReleaseDefaultTexture();
}

void CDynamicTexture::OnResetDevice()
{
    CreateDefaultTexture();
}

bool CDynamicTexture::CreateDefaultTexture()
{
    if (FAILED(m_device->CreateTexture(m_width, m_height, 1, 
        D3DUSAGE_DYNAMIC, m_format, 
        D3DPOOL_DEFAULT, &m_textureDefault, NULL)))
    {
        MessageBox(NULL, "Create Texture Error", "ERROR", 0);
        return false;
    }
    if (FAILED(m_textureDefault->GetSurfaceLevel(0, &m_surfaceDefault)))
    {
        m_textureDefault->Release();
        m_textureDefault = NULL;
        MessageBox(NULL, "Get Surface Error", "ERROR", 0);
        return false;
    } 
    return true;
}