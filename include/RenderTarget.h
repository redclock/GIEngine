#pragma once
#include <d3dx9.h>
#include "Game2D.h"

class CRenderTarget
{
protected:
    LPDIRECT3DTEXTURE9 m_texture;
    LPDIRECT3DSURFACE9 m_surface;
    LPDIRECT3DSURFACE9 m_dsurface;
    D3DVIEWPORT9 m_viewport;

    LPDIRECT3DSURFACE9 m_oldSurface;
    LPDIRECT3DSURFACE9 m_oldDsurface;
    D3DVIEWPORT9 m_oldViewport;

    UINT m_width;
    UINT m_height;
    D3DFORMAT m_format;
    D3DFORMAT m_dformat;
    DWORD m_index;
    int m_level;

    LPDIRECT3DDEVICE9 m_device;
    void CreateDepthSurface(D3DFORMAT format);
    void InitViewPort();

public:
    CRenderTarget();
    ~CRenderTarget();
    bool CreateTexture(LPDIRECT3DDEVICE9 device, UINT width, UINT height, 
        D3DFORMAT format, D3DFORMAT depthFormat = D3DFMT_UNKNOWN, UINT levels = 1,
        DWORD usage = NULL);
    void ReleaseTexture();
    LPDIRECT3DTEXTURE9 GetTexture() const { return m_texture; }
    LPDIRECT3DSURFACE9 GetSurface() const { return m_surface; }
    LPDIRECT3DSURFACE9 GetDepthSurface() const { return m_dsurface; }

    UINT GetWidth() const { return m_width; }
    UINT GetHeight() const { return m_height; }
    bool BeginRenderTarget(DWORD index);
    bool EndRenderTarget();
    bool ApplyDepthBuffer();
    bool RestoreDepthBuffer();
    bool UpdateSurface(int level);
    int GetSurfaceLevel() const { return m_level; }

    void Fill(CGame2D* g2d, int x, int y, int w, int h, 
        float color[4]);
    void Fill(float r, float g, float b, float a);

};