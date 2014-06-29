#include "Skybox.h"
#include <math.h>
#include "GameCommon.h"
#include "CubeBox.h"

void CSkyBox::setVertex(VERTEX_BOX& v, FLOAT x, FLOAT y, FLOAT z, DWORD color, FLOAT tu, FLOAT tv)
{
	v.x = x*m_wx - m_wx/2; v.y = y*m_wy - m_wy/2; v.z = z*m_wz - m_wz/2;
	v.color = color;
	v.tu = tu; v.tv = tv;
}

void CSkyBox::initVertices()
{    
	int r = 255, g = 255, b =255;
	DWORD dwColor = 0xff000000 + (r<<16) + (g<<8) + (b<<0);
	setVertex(m_Vertices[0], 1, 1, 0, dwColor, 1, 0);
	setVertex(m_Vertices[1], 0, 1, 0, dwColor, 0, 0);
	setVertex(m_Vertices[2], 0, 0, 0, dwColor, 0, 1);
	setVertex(m_Vertices[3], 1, 0, 0, dwColor, 1, 1);

	setVertex(m_Vertices[4], 0, 1, 1, dwColor, 1, 0);
	setVertex(m_Vertices[5], 1, 1, 1, dwColor, 0, 0);
	setVertex(m_Vertices[6], 1, 0, 1, dwColor, 0, 1);
	setVertex(m_Vertices[7], 0, 0, 1, dwColor, 1, 1);

	m_Vertices[9]=m_Vertices[19]=m_Vertices[0];
	m_Vertices[12]=m_Vertices[18]=m_Vertices[1];
	m_Vertices[15]=m_Vertices[23]=m_Vertices[2];
	m_Vertices[10]=m_Vertices[22]=m_Vertices[3];

	m_Vertices[13]=m_Vertices[17]=m_Vertices[4];
	m_Vertices[8]=m_Vertices[16]=m_Vertices[5];
	m_Vertices[11]=m_Vertices[21]=m_Vertices[6];
	m_Vertices[14]=m_Vertices[20]=m_Vertices[7];


	for (int i = 0; i < 24;)
	{
		m_Vertices[i].tu = 1;
		m_Vertices[i++].tv = 0;
		m_Vertices[i].tu = 0;
		m_Vertices[i++].tv = 0;
		m_Vertices[i].tu = 0;
		m_Vertices[i++].tv = 1;
		m_Vertices[i].tu = 1;
		m_Vertices[i++].tv = 1;
	}
}

CSkyBox::CSkyBox(FLOAT wx, FLOAT wy, FLOAT wz): CObject3D()
{
	m_wx = wx;
	m_wy = wy;
	m_wz = wz;
	initVertices();
	SetPosition(0, 0, 0);
	SetRotation(0, 0, 0);
}


CSkyBox::~CSkyBox(void)
{
	Destroy();
}

void CSkyBox::fillIndices(WORD *in, WORD a, WORD b, WORD c, WORD d)
{
	in[0] = a;    in[2] = c;    in[1] = b;
	in[3] = a;    in[5] = d;    in[4] = c;
}

HRESULT CSkyBox::Init(LPDIRECT3DDEVICE9 device)
{
	CObject3D::Init(device);

	WORD Indices[6*2*3];
	for (int i = 0, j = 0; i < 36; i+=6, j+=4){
		fillIndices(Indices+i, j, j+1, j+2, j+3);
	}


	DWORD SizeVertices = sizeof(m_Vertices);
	DWORD SizeIndices =  sizeof(Indices);

	if ( FAILED(m_device->CreateVertexBuffer(
		SizeVertices, 
		D3DUSAGE_WRITEONLY,
		VERTEX_BOX::FVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL))) return E_FAIL;

	VERTEX_BOX *pVertices;
	if ( FAILED(m_pVB->Lock( 0, SizeVertices, (VOID **)&pVertices, 0 ) ) ) return E_FAIL;
	memcpy(pVertices, m_Vertices, SizeVertices);
	m_pVB->Unlock();

	if ( FAILED(m_device->CreateIndexBuffer(SizeIndices, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB,                 NULL) ) )
		return E_FAIL;

	VOID * pIndices;
	if ( FAILED(m_pIB->Lock( 0, SizeIndices, (VOID **)&pIndices, 0 ) ) ) return E_FAIL;
	memcpy(pIndices, Indices, SizeIndices);
	m_pIB->Unlock();
	return S_OK;
}

void CSkyBox::Render()
{
	//m_device->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
	//m_device->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

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
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_BOX));
	m_device->SetIndices(m_pIB);
	m_device->SetFVF(VERTEX_BOX::FVF);
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
		0,
		0,
		8*3,
		0,
		6*2);

	m_device->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	m_device->SetTransform( D3DTS_VIEW,      &matViewSave );

}
