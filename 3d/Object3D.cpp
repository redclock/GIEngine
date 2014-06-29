#include "Object3D.h"
#include "GameCommon.h"

CObject3D::CObject3D(void)
{
	m_device = NULL;
	m_pVB = NULL;
	m_pIB = NULL;
    m_vertexSize = sizeof(VERTEX_TEX1);
    m_indexSize = sizeof(WORD);
    m_vertexCount = 0;
    m_indexCount = 0;
    m_x = 0; m_y = 0; m_z = 0;
    m_roll = 0; m_pitch = 0; m_yaw = 0;
    m_isVisible = true;
    D3DXMatrixIdentity(&m_matExtra);
}

CObject3D::~CObject3D(void)
{

}

HRESULT CObject3D::Init(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
	return S_OK;
}

void CObject3D::SetPosition(FLOAT x, FLOAT y, FLOAT z)
{
    m_x = x; m_y = y; m_z = z;
}

void CObject3D::SetRotation(FLOAT roll, FLOAT pitch, FLOAT yaw)
{
    m_roll = roll; m_pitch = pitch; m_yaw = yaw;
}

void CObject3D::UpdateMatrix()
{
    D3DXQUATERNION qR;
    
    Point3 center = m_boundBox.center();

    D3DXMATRIX matLocal;
    D3DXMATRIX matLocal2;
    D3DXMatrixTranslation(&matLocal, -center.x, -center.y, -center.z);
    D3DXMatrixTranslation(&matLocal2, center.x, center.y, center.z);

    D3DXQuaternionRotationYawPitchRoll(&qR,m_yaw, m_pitch, m_roll);
    D3DXMatrixRotationQuaternion(&m_matRot, &qR);
    D3DXMatrixTranslation(&m_matWorld, m_x, m_y, m_z);
    m_matWorld = matLocal * m_matRot * matLocal2 * m_matWorld;  
    m_matWorld *= m_matExtra;
}

void CObject3D::Destroy()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
}

void CObject3D::ComputeBoundBox(const D3DXMATRIX* mat)
{
    unsigned char* buf;
    if (m_pVB == NULL)
        return;
    if (FAILED(m_pVB->Lock(0, 0, (VOID **)&buf, D3DLOCK_READONLY)))
        return;
    for (size_t i = 0; i < m_vertexCount; i++)
    {
        Point3* p = (Point3*) buf;
        if (mat)
        {
            D3DXVECTOR4 pp;
            D3DXVec3Transform(&pp, (D3DXVECTOR3*) p, mat);
            p = (Point3 *) &pp;
        }
        if (i == 0)
            m_boundBox = BoundBox(*p);
        else
            m_boundBox.unionWithPoint(*p);
        buf += m_vertexSize;
    }
    m_pVB->Unlock();
}


int getId(void* buf, size_t indexSize)
{
    if (indexSize == 2)
        return *(WORD *) buf;
    else
        return *(DWORD *) buf;
}

float CObject3D::ComputeArea(const D3DXMATRIX* mat)
{
    unsigned char* vbuf;
    unsigned char* ibuf;

    if (FAILED(m_pVB->Lock(0, 0, (VOID **)&vbuf, D3DLOCK_READONLY)))
        return 0;
    if (FAILED(m_pIB->Lock(0, 0, (VOID **)&ibuf, D3DLOCK_READONLY)))
        return 0;
    size_t index = 0;
    float area = 0;
    while (index < m_indexCount)
    {
        int face1 = getId(ibuf, m_indexSize);
        ibuf += m_indexSize;
        int face2 = getId(ibuf, m_indexSize);
        ibuf += m_indexSize;
        int face3 = getId(ibuf, m_indexSize);
        ibuf += m_indexSize;

        Point3* p1 = (Point3*) (vbuf + m_vertexSize * face1);
        Point3* p2 = (Point3*) (vbuf + m_vertexSize * face2);
        Point3* p3 = (Point3*) (vbuf + m_vertexSize * face3);
        Point3 cross = (*p1 - *p3) % (*p2 - *p3);
        area += cross.GetLength() * 0.5f;
        index += 3;
    }
    m_pVB->Unlock();
    m_pIB->Unlock();
    return area;
}

/**
* Creates random dx, dy value from given random number u1 and u2 using
* triangle sample type.
*/
inline void uniformSampleTriangle(float u1, float u2,
                                  float* dx, float* dy)
{
    float su1 = sqrtf(u1);
    *dx = 1.0f - su1;
    *dy = u2 * su1;
}

/**
* Returns sample count by weight.
*/
inline size_t sampleCount(size_t count, float weight, float u)
{
    float d = count * weight;
    size_t scount = static_cast<size_t>(d);
    d -= scount;
    if (u < d)
        ++scount;
    return scount;
}

size_t CObject3D::SamplePoints( size_t nSamples, size_t nMax, Point3* positions, Point3* normals )
{
    unsigned char* vbuf;
    unsigned char* ibuf;

    if (FAILED(m_pVB->Lock(0, 0, (VOID **)&vbuf, D3DLOCK_READONLY)))
        return 0;
    if (FAILED(m_pIB->Lock(0, 0, (VOID **)&ibuf, D3DLOCK_READONLY)))
        return 0;
    size_t index = 0;
    float totlaArea = ComputeArea();
    size_t nSampled = 0;

    while (index < m_indexCount)
    {
        int face1 = getId(ibuf, m_indexSize);
        ibuf += m_indexSize;
        int face2 = getId(ibuf, m_indexSize);
        ibuf += m_indexSize;
        int face3 = getId(ibuf, m_indexSize);
        ibuf += m_indexSize;

        Point3* p1 = (Point3*) (vbuf + m_vertexSize * face1);
        Point3* n1 = (Point3*) (vbuf + m_vertexSize * face1 + sizeof(Point3));
        Point3* p2 = (Point3*) (vbuf + m_vertexSize * face2);
        Point3* n2 = (Point3*) (vbuf + m_vertexSize * face2 + sizeof(Point3));
        Point3* p3 = (Point3*) (vbuf + m_vertexSize * face3);
        Point3* n3 = (Point3*) (vbuf + m_vertexSize * face3 + sizeof(Point3));
        
        Point3 cross = (*p1 - *p3) % (*p2 - *p3);
        float area = cross.GetLength() * 0.5f;
        
        size_t sampd = sampleCount(nSamples, area / totlaArea, RandomRange(0, 1));
        sampd = min(sampd, nMax - nSampled);
        for (size_t j = 0; j < sampd; ++j)
        {
            float t1, t2;
            uniformSampleTriangle(RandomRange(0, 1), RandomRange(0, 1), &t1, &t2);
            positions[nSampled] = t1 * *p1 + t2 * *p2 + (1.0f - t1 - t2) * *p3;
            normals[nSampled] = t1 * *n1 + t2 * *n2 + (1.0f - t1 - t2) * *n3;

            ++nSampled;
        }

        index += 3;
    }
    m_pVB->Unlock();
    m_pIB->Unlock();
    return nSampled;
}