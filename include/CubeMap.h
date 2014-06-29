#pragma once
#include <d3dx9.h>
#include "Point3.h"
#include "RenderTarget.h"

class CCubeMap : public CRenderTarget
{
private:
    LPDIRECT3DCUBETEXTURE9 m_cubeMap;

    Point3 m_center;
    D3DXMATRIX m_matView;
    D3DXMATRIX m_matProj;

    UINT m_size;

public:
    CCubeMap();
    ~CCubeMap();

    bool CreateTexture(UINT size, D3DFORMAT format, 
        D3DFORMAT depthFormat = D3DFMT_UNKNOWN, UINT levels = 1,
        DWORD usage = NULL);

    void SetCenter(float x, float y, float z);
    const Point3& GetCenter() const { return m_center; }

    bool UpdateSurface(int face, int level);

    LPDIRECT3DCUBETEXTURE9 GetTexture() const { return m_cubeMap; };

    const D3DXMATRIX GetViewMatrix(int face) const;
    const D3DXMATRIX GetProjMatrix() const { return m_matProj; }
    UINT GetSize() const { return m_size; }
};

extern float CUBE_LOOK_VECTOR[6][3];
