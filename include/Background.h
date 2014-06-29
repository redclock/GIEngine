#pragma once
#include <d3dx9.h>
#include "Point3.h"

class CBackground
{
private:
    float m_color[3];
    LPD3DXEFFECT m_effect;
public:    
    CBackground();
    ~CBackground();

    void SetBackColor(float r, float g, float b);
    void SetEnvMap(LPDIRECT3DTEXTURE9 tex);
    D3DXMATRIX ComputeTexToDirMatrix(const Point3& eyePos, const D3DXMATRIX& matView, const D3DXMATRIX& matProj);
    void Render(LPDIRECT3DTEXTURE9 diffuseTex, UINT width, UINT height);
};