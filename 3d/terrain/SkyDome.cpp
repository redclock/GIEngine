#include "SkyDome.h"
#include "GameEngine.h"
void CSkyDome::initVertices(VERTEX_SKYDOME * pVertices)
{    
	float fRadAngle = (90 - m_fVertSweep) / 180 * D3DX_PI;
	float fShpereRadius = m_fRadius / cos (fRadAngle);

	float fYAdj = fShpereRadius * sin (fRadAngle);

	//
	// From the resolution, compute the angular sweep of one section
	// of the dome
	//

	float fHorzSweep = D3DX_PI * 2 / (4 * m_nResolution);

	//
	// Adjust the vertical resolution
	//

	float fVertSweep = m_fVertSweep / 180 * D3DX_PI / m_nResolution;

	//
	// Starting from the top, populate with nResolution number of rings
	//
	//
	// Starting from the top, populate with nResolution number of rings
	//

	int nVertex = 0;
	for (int i = 0; i < m_nResolution + 1; i++)
	{

		//
		// Compute the vertex that will be rotated around to make a ring
		//

		D3DXVECTOR4 vPoint (0, fShpereRadius, 0, 1);
		D3DXMATRIXA16 matRot;
		D3DXMatrixRotationZ(&matRot, fVertSweep * i);
		D3DXVec4Transform(&vPoint, &vPoint, &matRot);

		vPoint.y = (vPoint.y - fYAdj) * m_fHeightScale;

		//
		// Loop through the ring creating the points
		//

		for (int j = 0; j < m_nVertexInLine; j++)
		{

			//
			// Map the point
			//
			D3DXVECTOR4 pos;
			D3DXMatrixRotationY(&matRot, -fHorzSweep * j);
			D3DXVec4Transform(&pos, &vPoint, &matRot);			
			pVertices[nVertex].pos = Point3(m_vOrigin.x + pos.x, m_vOrigin.y + pos.y, m_vOrigin.z + pos.z);
			pVertices[nVertex].color = 0xFFFFFFFF;
			pVertices[nVertex].tv = (i) / (float) m_nResolution;
			pVertices[nVertex].tu = (j) / (float) (4 *m_nResolution );
			nVertex++;
		}
	}

	//
	// All done
	//

	return;

}

void CSkyDome::initIndices(WORD * pIndices)
{
	int nIndex = 0;

	//triangle strip
	
	//int nDelta = m_nResolution * 4 + 1;
	for (int j = 1; j < m_nResolution + 1; j++)
	{
		int nStart = m_nVertexInLine * j;
		int nEnd = nStart + m_nVertexInLine;
		for (int i = nStart; i < nEnd; i++)
		{
			pIndices[nIndex++] = i - m_nVertexInLine;
			pIndices[nIndex++] = i;
		}
		pIndices[nIndex++] = nStart - m_nVertexInLine;
		pIndices[nIndex++] = nStart;
	}
}

CSkyDome::CSkyDome()
{

}

CSkyDome::~CSkyDome()
{
	Destroy();
}

HRESULT CSkyDome::Init(LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 tex, 
					   int nResolution, float fVertSweep, float fRadius, 
					   float fHeightScale, const Point3 &vOrigin)
{
	CObject3D::Init(device, tex);
	
	//
	// Save the generation arguments
	//

	m_nResolution = nResolution;
	m_fVertSweep = fVertSweep;
	m_fRadius = fRadius;
	m_fHeightScale = fHeightScale;
	m_vOrigin = vOrigin;


	//
	// Compute the number of vertices
	//
	m_nVertexInLine = (4 * nResolution + 1);

	m_nVertex = m_nVertexInLine * (nResolution + 1);

	m_nIndex = (m_nResolution + 1) * (m_nVertexInLine * 2 + 2);

	DWORD SizeVertices = sizeof(VERTEX_SKYDOME) * m_nVertex;
	DWORD SizeIndices =  sizeof(WORD) * m_nIndex;

	if ( FAILED(m_device->CreateVertexBuffer(
		SizeVertices, 
		D3DUSAGE_WRITEONLY,
		VERTEX_SKYDOME::FVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL))) return E_FAIL;

	VERTEX_SKYDOME *pVertices;
	if ( FAILED(m_pVB->Lock( 0, SizeVertices, (VOID **)&pVertices, 0 ) ) ) return E_FAIL;
	initVertices(pVertices);
	m_pVB->Unlock();

	if ( FAILED(m_device->CreateIndexBuffer(SizeIndices, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL) ) )
		return E_FAIL;

	WORD * pIndices;
	if ( FAILED(m_pIB->Lock( 0, SizeIndices, (VOID **)&pIndices, 0 ) ) ) return E_FAIL;
	initIndices(pIndices);
	m_pIB->Unlock();
	return S_OK;


}

void CSkyDome::Render()
{
	//m_device->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
	//m_device->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );
	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	m_device->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
	m_device->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
	D3DXMATRIXA16 matView, matViewSave;
	m_device->GetTransform( D3DTS_VIEW, &matViewSave );
	matView = matViewSave;
	matView._41 = 0.0f; matView._42 = 0.0f; matView._43 = 0.0f;
	m_device->SetTransform( D3DTS_VIEW,      &matView );
	m_device->SetRenderState( D3DRS_ZENABLE, FALSE );
	// Some cards do not disable writing to Z when 
	// D3DRS_ZENABLE is FALSE. So do it explicitly
	m_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	m_device->SetTransform( D3DTS_WORLD, &m_matWorld );
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_SKYDOME));
	m_device->SetIndices(m_pIB);
	m_device->SetFVF(VERTEX_SKYDOME::FVF);

	TextureSelect(m_pTexture);

	for (int j = 0; j < m_nResolution; j++)
	{
		if (FAILED(m_device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 
			0,
			0,
			m_nVertex,
			j * ((m_nVertexInLine * 2 + 2)),
			m_nVertexInLine * 2)))
			MessageBox(GetWindow(), "error rendering sky", "error", 0);
	}

	m_device->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	m_device->SetTransform( D3DTS_VIEW,      &matViewSave );

}
