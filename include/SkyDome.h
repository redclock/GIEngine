#pragma once
#include "Point3.h"
#include "MyLib.h"
#include "Object3D.h"

struct VERTEX_SKYDOME
{
	Point3 pos;
	DWORD color;
	FLOAT  tu, tv;   // Vertex texture coordinates
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
};

class CSkyDome: public CObject3D
{
private:
	int			m_nVertex;
	int			m_nResolution;

	int			m_nVertexInLine;

	float		m_fVertSweep;
	float		m_fRadius;
	float		m_fHeightScale;
	Point3		m_vOrigin;

	int			m_nIndex;

	void setVertex(VERTEX_SKYDOME& v, FLOAT x, FLOAT y, FLOAT z, DWORD color, FLOAT tu, FLOAT tv);
	void initVertices(VERTEX_SKYDOME * pVertices);
	void initIndices(WORD * pIndices);

public:
	HRESULT Init(LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 tex, 
				int nResolution, float fVertSweep, float fRadius, 
				float fHeightScale, const Point3 &vOrigin);
	void Render();

	CSkyDome();
	~CSkyDome(void);
};
