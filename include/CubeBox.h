/************************************************************************/
/* Direct3D GameEngine                                                  */
/* 立方体模型                 编写： 姚春晖                             */
/************************************************************************/

#pragma once
#include <Windows.h>

#include <D3DX9.h>

#include "Object3D.h"

struct VERTEX_BOX
{
	FLOAT x, y, z;
	FLOAT nx, ny, nz;
	DWORD color;
	FLOAT  tu, tv;   // Vertex texture coordinates
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL| D3DFVF_DIFFUSE  | D3DFVF_TEX1;
};


class CCubeBox: public CObject3D
{
private:
    FLOAT m_wx, m_wy, m_wz;
    FLOAT m_tilew, m_tileh;
    bool m_tiled;
    bool m_inner;

    VERTEX_BOX m_Vertices[4*6];


    void fillIndices(WORD *in, WORD a, WORD b, WORD c, WORD d);
    void setVertex(VERTEX_BOX& v, FLOAT x, FLOAT y, FLOAT z, DWORD color, FLOAT tu, FLOAT tv);
    void initVertices();

public:
    HRESULT Init(LPDIRECT3DDEVICE9 device);
    void Render();
    void ComputeBoundBox();

    bool Collision(D3DXVECTOR3& vGo, D3DXVECTOR3& vPos);
    CCubeBox(FLOAT wx, FLOAT wy, FLOAT wz, bool inner = false);
    CCubeBox(FLOAT wx, FLOAT wy, FLOAT wz, FLOAT tilew, FLOAT tileh, bool inner = false);
    ~CCubeBox(void);
};
