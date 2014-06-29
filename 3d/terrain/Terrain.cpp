#include "Terrain.h"
#include "MyDIB.h"
#include "LightMap.h"

CTerrain::CTerrain(int depth, float cellLen):
	CObject3D(),
	m_length((1 << depth) + 1), 
	m_cellLen(cellLen),
	m_heights(m_length, m_length),
	m_normals(m_length, m_length),
	m_quadtree(depth)
{
	m_heights.ZeroArray();
}

CTerrain::~CTerrain()
{
	Destroy();
}

void CTerrain::CalulateIndices(WORD * idx)
{
	int j = 0;

	for( int i = 0; i < (m_length - 1) * m_length; i++ )
	{
		if ((i + 1) % (m_length) == 0) continue;
		idx[j++] = i;
		idx[j++] = i + m_length + 1;
		idx[j++] = i + 1;
		idx[j++] = i;
		idx[j++] = i + m_length;
		idx[j++] = i + m_length + 1;
	}
}

void CTerrain::CalulateVertices(VERTEX_TERRAIN *pVertices)
{
	Array2D<VERTEX_TERRAIN> vertices(m_length, m_length, pVertices);


	for (int r = 0; r < m_length; r++)
		for (int c = 0; c < m_length; c++)
		{
			vertices[r][c].pos.x = c * m_cellLen;
			vertices[r][c].pos.z = r * m_cellLen;
			vertices[r][c].pos.y = m_heights[r][c];
			
			float alpha;
			alpha = vertices[r][c].pos.y / 10;
			if (alpha > 1) alpha = 1;
			if (alpha < 0) alpha = 0;
			vertices[r][c].color = (int)(alpha * 255) << 24;
			vertices[r][c].tu1 = m_texFactor1 * c / (float) m_length;
			vertices[r][c].tv1 = m_texFactor1 * r / (float) m_length;
			vertices[r][c].tu2 = m_texFactor2 * c / (float) m_length;
			vertices[r][c].tv2 = m_texFactor2 * r / (float) m_length;
			vertices[r][c].tu3 = c / (float) m_length;
			vertices[r][c].tv3 = r / (float) m_length;
		}

}

void CTerrain::CalulateNormals(VERTEX_TERRAIN *pVertices)
{
	Array2D<VERTEX_TERRAIN> vertices(m_length, m_length, pVertices);

	for (int r = 0; r < m_length; r++)
		for (int c = 0; c < m_length; c++)
		{
			Point3 p(0, 0, 0);

			if (r > 0 && c > 0)
				p += ((vertices[r - 1][c].pos - vertices[r][c].pos) 
				% (vertices[r][c - 1].pos - vertices[r][c].pos)).Normalize();

			if (r > 0 && c < m_length - 1)
				p += ((vertices[r][c + 1].pos - vertices[r][c].pos) 
				% (vertices[r - 1][c].pos - vertices[r][c].pos)).Normalize();

			if (r < m_length - 1 && c < m_length - 1)
				p += ((vertices[r + 1][c].pos - vertices[r][c].pos) 
				% (vertices[r][c + 1].pos - vertices[r][c].pos)).Normalize();

			if (r < m_length - 1 && c > 0)
				p += ((vertices[r][c - 1].pos - vertices[r][c].pos) 
				% (vertices[r + 1][c].pos - vertices[r][c].pos)).Normalize();
			m_normals[r][c] = p.Normalize();
		}
}

HRESULT CTerrain::Init(LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 tex, float f1, 
					   LPDIRECT3DTEXTURE9 dtex, float f2, LPDIRECT3DTEXTURE9 lightmap)
{
	CObject3D::Init(device, tex);

	m_pDetailTexture = dtex;
	m_pLightmap = lightmap;
	m_texFactor1 = f1;
	m_texFactor2 = f2;

	DWORD SizeVertices = (m_length) * (m_length) * sizeof(VERTEX_TERRAIN);
	DWORD SizeIndices =  6 * (m_length - 1) * (m_length - 1) * sizeof(DWORD);
	m_indexCount = (m_length - 1) * (m_length - 1) * 6;
	if ( FAILED(m_device->CreateVertexBuffer(
		SizeVertices, 
		D3DUSAGE_WRITEONLY,
		VERTEX_TERRAIN::FVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL))) return E_FAIL;

	VERTEX_TERRAIN *pVertices;
	if ( FAILED(m_pVB->Lock( 0, SizeVertices, (VOID **)&pVertices, 0 ) ) ) return E_FAIL;
	CalulateVertices(pVertices);
	CalulateNormals(pVertices);
	m_pVB->Unlock();

	if ( FAILED(m_device->CreateIndexBuffer(SizeIndices, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pIB, NULL) ) )
		return E_FAIL;

	DWORD * pIndices;

	m_quadtree.BuildTree();
	m_quadtree.CalcRoughness(m_heights, m_cellLen);
	m_quadtree.ReLOD(0, 0);
	
	if ( FAILED(m_pIB->Lock( 0, SizeIndices, (VOID **)&pIndices, 0 ) ) ) return E_FAIL;
	m_indexCount = m_quadtree.Draw(pIndices);
	//CalulateIndices(pIndices);
	m_pIB->Unlock();
	return S_OK;
}

void CTerrain::Render()
{
	SetEffect(fxCopy);

	m_device->SetTexture(0, m_pTexture);
	m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	m_device->SetTexture(1, m_pDetailTexture);
	m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
	m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_BLENDDIFFUSEALPHA  );
	m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	m_device->SetTexture(2, m_pLightmap);
	m_device->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2);
	m_device->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_device->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );
	m_device->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_device->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );


	//TextureDiffuseBlend(m_pTexture, m_pDetailTexture);
	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_device->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_device->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	m_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
	m_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );
	m_device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
	m_device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );

	m_device->SetTransform( D3DTS_WORLD, &m_matWorld );
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_TERRAIN));
	m_device->SetIndices(m_pIB);
	m_device->SetFVF(VERTEX_TERRAIN::FVF);

	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
		0,
		0,
		(m_length) * (m_length),
		0,
		m_indexCount / 3
		);
}

float CTerrain::GetHeightAt(float x, float z) const
{
	int px = (int)(x / m_cellLen);
	int pz = (int)(z / m_cellLen);
	if (px < 0 || px >= m_length - 1) return 0;
	if (pz < 0 || pz >= m_length - 1) return 0;
	float dx = (x - px * m_cellLen) / m_cellLen;
	float dz = (z - pz * m_cellLen) / m_cellLen;
	float z1, z2, z3, z4;
	if (dx < dz)
	{
		z1 = m_heights[pz][px];
		z3 = m_heights[pz + 1][px];
		z4 = m_heights[pz + 1][px + 1];
		z2 = z1 + z4 - z3;		
	}else
	{
		z1 = m_heights[pz][px];
		z2 = m_heights[pz][px + 1];
		z4 = m_heights[pz + 1][px + 1];
		z3 = z1 + z4 - z2;
	}
	return (z1 * (1 - dx) + z2 * dx) * (1 - dz)
		+(z3 * (1 - dx) + z4 * dx) *dz;
}

bool CTerrain::LoadHeightMap(const char * filename, float maxHeight)
{
	float h = maxHeight / 255.0f;
	m_maxHeight = maxHeight;

	KDIB dib;
	if (! dib.LoadFile(filename)) return false;

	if (dib.GetWidth() < m_length || dib.GetHeight() < m_length)
	{
		return false;
	}
	switch(dib.GetBitCount())
	{
	case 8:
		for (int y = 0; y < m_length; y ++)
		{
			BYTE * line = dib.GetScanLine(y);
			for (int x = 0; x < m_length; x++)
			{
				m_heights[y][x] = line[x] * h;
			}
		}
		break;
	case 24:
		for (int y = 0; y < m_length; y ++)
		{
			BYTE * line = dib.GetScanLine(y);
			for (int x = 0; x < m_length; x ++)
			{
				m_heights[y][x] = (line[x * 3] * h + line[x * 3 + 1] * h + line[x * 3 + 2] * h) / 3;
			}
		}
		break;
	case 32:
		for (int y = 0; y < m_length; y ++)
		{
			BYTE * line = dib.GetScanLine(y);
			for (int x = 0; x < m_length; x++)
			{
				m_heights[y][x] = (line[x * 4] * h + line[x * 4 + 1] * h + line[x * 4 + 2] * h) / 3;
			}
		}
		break;
	default:
		return false;
	}

	return true;
}

void CTerrain::ReLOD(float x, float z)
{
	m_quadtree.ReLOD(z / m_cellLen, x / m_cellLen);

	DWORD SizeIndices =  6 * (m_length - 1) * (m_length - 1) * sizeof(DWORD);
	DWORD * pIndices;
	if ( FAILED(m_pIB->Lock( 0, SizeIndices, (VOID **)&pIndices, D3DLOCK_DISCARD) ) ) return;
	m_indexCount = m_quadtree.Draw(pIndices);
	m_pIB->Unlock();

}

void CTerrain::GenLightmap(const char * filename)
{
	VERTEX_TERRAIN *pVertices;
	DWORD SizeVertices = (m_length) * (m_length) * sizeof(VERTEX_TERRAIN);
	if ( FAILED(m_pVB->Lock( 0, SizeVertices, (VOID **)&pVertices, 0 ) ) ) return;
	
	Array2D<VERTEX_TERRAIN> vertices(m_length, m_length, pVertices);
	KDIB * lightmap = ::GenLightmap(m_cellLen, m_normals, vertices, Point3(1, 1, 1), 0.1f);
	lightmap->SaveFile(filename);
	delete lightmap;
	m_pVB->Unlock();
}