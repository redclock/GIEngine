#include ".\picgrid.h"

CPicGrid::CPicGrid(int rows, int cols, float rowlen, float collen): points(0, 0)
{
	m_idx = NULL;
	this->rowlen = rowlen;
	this->collen = collen;
	Reset(rows, cols, rowlen, collen);
}

CPicGrid::~CPicGrid(void)
{
	Release();
}

void CPicGrid::Reset(int rows, int cols, float rowlen, float collen)
{
	Release();
	points.Resize(rows, cols);
	float dr = 1.0f / (rows - 1);
	float dc = 1.0f / (cols - 1);
	float rectdr = rowlen / (rows - 1);
	float rectdc = collen / (cols - 1);


	m_idx = new WORD[(rows - 1) * (cols - 1) * 6];
	//计算点
	for (int r = 0; r < rows; r ++)
		for (int c = 0; c < cols; c++)
		{
			points[r][c].color = 0xFFFFFFFF;
			points[r][c].rhw = 1.0f;
			points[r][c].x = c * rectdc;
			points[r][c].y = r * rectdr;
			points[r][c].z = 0.5f;
			points[r][c].tu = c * dc;
			points[r][c].tv = r * dr;
		}

	//计算索引
	int j = 0;
	for( int i = 0; i < (rows - 1) * cols; i++ )
	{
		if ((i + 1) % cols == 0) continue;
		m_idx[j++] = i;
		m_idx[j++] = i + 1;
		m_idx[j++] = i + cols + 1;
		m_idx[j++] = i;
		m_idx[j++] = i + cols + 1;
		m_idx[j++] = i + cols;
	}
	m_idxCount = j;
}

void CPicGrid::Release()
{
	if (m_idx)
	{
		delete [] m_idx;
		m_idx = NULL;
	}
}

HRESULT CPicGrid::Render(LPDIRECT3DDEVICE9 device)
{
    device->SetFVF( VERTEX_GRID::FVF );

	device->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 
		points.GetRows() * points.GetCols(), m_idxCount / 3, m_idx, 
		D3DFMT_INDEX16, points.GetBuf(), sizeof(VERTEX_GRID) );

    return S_OK;
}
