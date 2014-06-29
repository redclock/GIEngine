#pragma once
#include <Windows.h>

#include <D3DX9.h>

#include "Object3D.h"
#include "Array2D.h"
#include "QuadTree.h"
#include "Point3.h"

struct VERTEX_TERRAIN
{
	Point3 pos;
	DWORD color;
	FLOAT  tu1, tv1; // Vertex texture coordinates
	FLOAT  tu2, tv2; // Vertex texture coordinates
	FLOAT  tu3, tv3; // Vertex texture coordinates
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX3;
};

class CTerrain: public CObject3D
{
protected:
	LPDIRECT3DTEXTURE9 m_pDetailTexture;
	LPDIRECT3DTEXTURE9 m_pLightmap;
	int m_length;

	float m_cellLen;

	//FLOAT *m_heights;

	float m_texFactor1, m_texFactor2;

	float m_maxHeight;
	Array2D<float> m_heights;
	Array2D<Point3> m_normals;
	CQuadTree m_quadtree;

	DWORD m_indexCount;

	void CalulateIndices(WORD * idx);
	void CalulateVertices(VERTEX_TERRAIN *pVertices);
	void CalulateNormals(VERTEX_TERRAIN *pVertices);
public:
	int GetLength() const { return m_length; };
	float GetCellLen() const { return m_cellLen; };
	void SetCellLen(float len) { m_cellLen = len; };

	float GetHeight(int r, int c) const { return m_heights[r][c]; };
	void  SetHeight(int r, int c, float h) { m_heights[r][c] = h; };
	Array2D<float> & GetHeights() { return m_heights; };


	float GetHeightAt(float x, float z) const;

	HRESULT Init(LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 tex, float f1,
					LPDIRECT3DTEXTURE9 dtex, float f2, LPDIRECT3DTEXTURE9 lightmap);
	void Render();
	void UpdateMesh();

	bool Collision(D3DXVECTOR3& vGo, D3DXVECTOR3& vPos){ return false; }; 

	bool LoadHeightMap(const char * filename, float maxHeight);
	
	void ReLOD(float x, float z);

	CTerrain(int depth, float cellLen);
	~CTerrain(void);

public:
	int TotalTriangle() const { return (m_length - 1) * (m_length - 1) * 2; }
	int RenderedTriangle() const { return m_indexCount / 3; } 
	void GenLightmap(const char * filename);
};
