#include "Face3D.h"
#include "GameEngine.h"

CFace3D::CFace3D(float w, float h): CObject3D()
{
	m_width = w;
	m_height = h;
	SetPosition(0, 0, 0);
	SetRotation(0, 0, 0);
}

CFace3D::~CFace3D()
{
	Destroy();
}

void CFace3D::setVertex(VERTEX_FACE& v, FLOAT x, FLOAT y, FLOAT z, DWORD color, FLOAT tu, FLOAT tv)
{
	v.x = x; v.y = y; v.z = z;
	v.color = color;
	v.tu = tu; v.tv = tv;
}

HRESULT CFace3D::Init(LPDIRECT3DDEVICE9 device)
{
	CObject3D::Init(device);

	DWORD SizeVertices = 4 * sizeof(VERTEX_FACE);

	if ( FAILED(m_device->CreateVertexBuffer(
		SizeVertices, 
		D3DUSAGE_WRITEONLY,
		VERTEX_FACE::FVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL))) return E_FAIL;

	VERTEX_FACE *pVertices;
	if ( FAILED(m_pVB->Lock( 0, SizeVertices, (VOID **)&pVertices, 0 ) ) ) return E_FAIL;
	setVertex(pVertices[0], -0.5f, 0.5f, 0, 0xFFFFFFFF, 0, 0); 
	setVertex(pVertices[1], 0.5f, 0.5f, 0, 0xFFFFFFFF, 1, 0); 
	setVertex(pVertices[2], 0.5f, -0.5f, 0, 0xFFFFFFFF, 1, 1); 
	setVertex(pVertices[3], -0.5f, -0.5f, 0, 0xFFFFFFFF, 0, 1); 
	m_pVB->Unlock();
	return S_OK;
}

void CFace3D::UpdateMatrix()
{
	D3DXMatrixTranslation(&m_matWorld, m_x, m_y, m_z);
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, m_width, m_height, 1);
	m_matWorld = matScale * m_matWorld;  
}

void CFace3D::UpdateMatrixBillboard()
{
	GetDevice()->GetTransform(D3DTS_VIEW, &m_matRot);
	m_matRot._41 = m_matRot._42 = m_matRot._43 = 0;
	D3DXMatrixInverse(&m_matRot, NULL, &m_matRot);
	D3DXMatrixTranslation(&m_matWorld, m_x, m_y, m_z);
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, m_width, m_height, 1);
	m_matWorld = matScale * m_matRot * m_matWorld;  
}


void CFace3D::Render()
{
	//m_device->SetTransform( D3DTS_WORLD, &m_matWorld );
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_FACE));
	m_device->SetFVF(VERTEX_FACE::FVF);
	m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}

CAnimationFace3D::CAnimationFace3D(float w, float h, int r, int c): CFace3D(w, h)
{
	m_clipRows = r;
	m_clipCols = c;
	m_time = 0;
	curFrame = 0;
	Delay = 0.1f;
	startFrame = 0;
	endFrame = r * c - 1;
}

CAnimationFace3D::~CAnimationFace3D()
{

}

void CAnimationFace3D::Reset()
{
	curFrame = startFrame;
	m_time = 0;
	float sw = 1.0f / m_clipCols;
	float sh = 1.0f / m_clipRows;
	float su = curFrame % m_clipCols * sw; 
	float sv = curFrame / m_clipCols * sh;
	DWORD SizeVertices = 4 * sizeof(VERTEX_FACE);
	VERTEX_FACE *pVertices;
	if ( FAILED(m_pVB->Lock( 0, SizeVertices, (VOID **)&pVertices, 0 ) ) ) return;
	pVertices[0].tu = su; pVertices[0].tv = sv;
	pVertices[1].tu = su + sw; pVertices[1].tv = sv;
	pVertices[2].tu = su + sw; pVertices[2].tv = sv + sh;
	pVertices[3].tu = su; pVertices[3].tv = sv + sh;
	m_pVB->Unlock();
}

void CAnimationFace3D::Animate(float elapsedTime)
{
	m_time += elapsedTime;
	if (m_time >= Delay)
	{
		m_time -= Delay;
		curFrame ++;
		if (curFrame >	endFrame) curFrame = startFrame;
		if (curFrame < startFrame) curFrame = endFrame;
		float sw = 1.0f / m_clipCols;
		float sh = 1.0f / m_clipRows;
		float su = curFrame % m_clipCols * sw; 
		float sv = curFrame / m_clipCols * sh;
		DWORD SizeVertices = 4 * sizeof(VERTEX_FACE);
		VERTEX_FACE *pVertices;
		if ( FAILED(m_pVB->Lock( 0, SizeVertices, (VOID **)&pVertices, 0 ) ) ) return;
		pVertices[0].tu = su; pVertices[0].tv = sv;
		pVertices[1].tu = su + sw; pVertices[1].tv = sv;
		pVertices[2].tu = su + sw; pVertices[2].tv = sv + sh;
		pVertices[3].tu = su; pVertices[3].tv = sv + sh;
		m_pVB->Unlock();
	}
}

CBillboard::CBillboard(float w, float h): CFace3D(w, h)
{
	m_width = w;
	m_height = h;
	SetPosition(0, 0, 0);
	SetRotation(0, 0, 0);
}

CBillboard::~CBillboard()
{
	Destroy();
}

void CBillboard::UpdateMatrix()
{
	GetDevice()->GetTransform(D3DTS_VIEW, &m_matRot);
	m_matRot._41 = m_matRot._42 = m_matRot._43 = 0;
	D3DXMatrixInverse(&m_matRot, NULL, &m_matRot);
	D3DXMatrixTranslation(&m_matWorld, m_x, m_y, m_z);
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, m_width, m_height, 1);
	m_matWorld = matScale * m_matRot * m_matWorld;  
}

