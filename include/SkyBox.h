#pragma once
#include <D3DX9.h>

#include "MyLib.h"
#include "Object3D.h"
#include "CubeBox.h"

class CSkyBox: public CObject3D
{
private:
	FLOAT m_wx, m_wy, m_wz;

	VERTEX_BOX m_Vertices[4*6];


	void fillIndices(WORD *in, WORD a, WORD b, WORD c, WORD d);
	void setVertex(VERTEX_BOX& v, FLOAT x, FLOAT y, FLOAT z, DWORD color, FLOAT tu, FLOAT tv);
	void initVertices();
public:
	HRESULT Init(LPDIRECT3DDEVICE9 device);
	void Render();

	CSkyBox(FLOAT wx, FLOAT wy, FLOAT wz);
	~CSkyBox(void);
};
