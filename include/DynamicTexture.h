#pragma once
#include <d3dx9.h>
class CDynamicTexture
{
private:
    LPDIRECT3DTEXTURE9 m_textureDefault;
    LPDIRECT3DSURFACE9 m_surfaceDefault;
    LPDIRECT3DTEXTURE9 m_textureSystem;
    LPDIRECT3DSURFACE9 m_surfaceSystem;
    UINT m_width;
    UINT m_height;
    D3DFORMAT m_format;
    LPDIRECT3DDEVICE9 m_device;

    void ReleaseDefaultTexture();
    bool CreateDefaultTexture();

public:
    CDynamicTexture();
    ~CDynamicTexture();
    bool CreateTexture(LPDIRECT3DDEVICE9 device, UINT width, UINT height, D3DFORMAT format = D3DFMT_X8R8G8B8);
    void ReleaseTextures();
    LPDIRECT3DTEXTURE9 GetTextureDefault() const { return m_textureDefault; }
    LPDIRECT3DSURFACE9 GetSurfaceDefault() const { return m_surfaceDefault; }
    LPDIRECT3DTEXTURE9 GetTextureSystem() const { return m_textureSystem; }
    LPDIRECT3DSURFACE9 GetSurfaceSystem() const { return m_surfaceSystem; }
    UINT GetWidth() const { return m_width; }
    UINT GetHeight() const { return m_height; }
    bool LockRect(D3DLOCKED_RECT *pLockedRect, const RECT* rect, DWORD flags);
    bool UnlockRect();
    bool Update(const RECT* src, const POINT* dst);
    void OnLostDevice();
    void OnResetDevice();
};