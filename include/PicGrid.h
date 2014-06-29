#pragma once
#include <d3dx9.h>
#include "MyLib.h"
#include "Array2D.h"

typedef VERTEX_XYZRHWTEX1 VERTEX_GRID; 
class CPicGrid
{
private:
	WORD * m_idx;
	int m_idxCount;
	void Release();
public:
	float rowlen;
	float collen;
	Array2D<VERTEX_GRID> points;	

	HRESULT Render(LPDIRECT3DDEVICE9 device);
	void Reset(int rows, int cols, float rowlen, float collen);
	CPicGrid(int rows, int cols, float rowlen, float collen);
	~CPicGrid(void);
};
