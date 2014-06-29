#include "CubeBox.h"
#include <math.h>
#include "GameCommon.h"


void CCubeBox::setVertex(VERTEX_BOX& v, FLOAT x, FLOAT y, FLOAT z,  
						 DWORD color, FLOAT tu, FLOAT tv)
{
    v.x = x*m_wx - m_wx/2; v.y = y*m_wy - m_wy/2; v.z = z*m_wz - m_wz/2;
	float nx, ny, nz;
	nx = x == 0? -1.0f: 1.0f;
	ny = y == 0? -1.0f: 1.0f;
	nz = z == 0? -1.0f: 1.0f;
	float len = sqrt(nx*nx + ny*ny + nz*nz);
	v.nx = nx / len; v.ny = ny / len; v.nz = nz / len;
    if (m_inner)
    {
        v.nx = -v.nx;
        v.ny = -v.ny;
        v.nz = -v.nz;
    }
    v.color = color;
    v.tu = tu; v.tv = tv;
}

void CCubeBox::initVertices()
{    
    int r = 255, g = 255, b = 255;
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
   
    if (m_tiled)
    {
        FLOAT tex_x[6] = 
        {
            m_wx / m_tilew, 
            m_wx / m_tilew,
            m_wz / m_tilew,
            m_wz / m_tilew,
            m_wx / m_tilew,
            m_wx / m_tilew
        };
        FLOAT tex_y[6] = 
        {
            m_wy / m_tileh, 
            m_wy / m_tileh,
            m_wy / m_tileh,
            m_wy / m_tileh,
            m_wz / m_tileh,
            m_wz / m_tileh
        };
        for (int i = 0, j =0; i < 24; j++)
        {
            m_Vertices[i].tu = tex_x[j];
            m_Vertices[i++].tv = 0;
            m_Vertices[i].tu = 0;
            m_Vertices[i++].tv = 0;
            m_Vertices[i].tu = 0;
            m_Vertices[i++].tv = tex_y[j];
            m_Vertices[i].tu = tex_x[j];
            m_Vertices[i++].tv = tex_y[j];
        }
    }else{
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
}

CCubeBox::CCubeBox(FLOAT wx, FLOAT wy, FLOAT wz, bool inner): CObject3D()
{
    m_wx = wx;
    m_wy = wy;
    m_wz = wz;
    m_inner = inner;
    m_tiled = false;
    m_vertexCount = 4 * 6;
    m_indexCount = 6 * 2 * 3;
    m_vertexSize = sizeof(VERTEX_BOX);
    m_indexSize = sizeof(WORD);
    initVertices();
    SetPosition(0, 0, 0);
    SetRotation(0, 0, 0);
}

CCubeBox::CCubeBox(FLOAT wx, FLOAT wy, FLOAT wz, FLOAT tilew, FLOAT tileh, bool inner): CObject3D()
{
    m_wx = wx;
    m_wy = wy;
    m_wz = wz;
    m_tileh = tileh;
    m_tilew = tilew;
    m_inner = inner;
    if (tileh <=0 || tilew <=0) m_tiled = false;
    m_tiled = true;
    initVertices();
    SetPosition(0, 0, 0);
    SetRotation(0, 0, 0);
}

CCubeBox::~CCubeBox(void)
{
	Destroy();
}

void CCubeBox::fillIndices(WORD *in, WORD a, WORD b, WORD c, WORD d)
{
    if (m_inner)
    {
        in[0] = c;    in[1] = a;    in[2] = b;
        in[3] = d;    in[4] = a;    in[5] = c;
    }
    else
    {
        in[0] = a;    in[1] = c;    in[2] = b;
        in[3] = a;    in[4] = d;    in[5] = c;
    }
}

HRESULT CCubeBox::Init(LPDIRECT3DDEVICE9 device)
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

bool CCubeBox::Collision(D3DXVECTOR3& vGo, D3DXVECTOR3& vPos)
{
    FLOAT StdDist[3] = { m_wx/2, m_wy/2, m_wz/2 };
    //矩形中心
    D3DXVECTOR3 vCenter( m_x + StdDist[0], m_y + StdDist[1], m_z + StdDist[2]);
    //原位置指向中心
    D3DXVECTOR3 D = vCenter - vPos; 
    //若方向相反
    //if (D3DXVec3Dot(&vGo, &D) <= 0) return false;
    //长宽高单位向量
    D3DXVECTOR3 vSub[3];
    vSub[0] = D3DXVECTOR3( 1, 0, 0 );
    vSub[1] = D3DXVECTOR3( 0, 1, 0 );
    vSub[2] = D3DXVECTOR3( 0, 0, 1 );
    //前进方向分量
    FLOAT vGoSub[3];
    //距中心分量
    FLOAT vDSub[3];
    for (int i = 0; i < 3; i++)
    {
        D3DXVec3TransformCoord(&vSub[i], &vSub[i], &m_matRot);
        vGoSub[i] = D3DXVec3Dot(&vGo, &vSub[i]);
        vDSub[i] = D3DXVec3Dot(&D, &vSub[i]);
    }
    //二者的差距
    FLOAT inner[3];
    for (int i = 0; i<3; i++)
    {
        inner[i] = StdDist[i] - abs(vDSub[i] - vGoSub[i])+0.5f;
        //有一个<0，就没碰
        if (inner[i] < 0) return false;
    }
    //三个都大于0
    int Min = 0;
    FLOAT s = 100000.0f;
    for (int i =0; i < 3; i++)
    {
        if (inner[i] / abs(vGoSub[i]) < s)
        {
            s = inner[i] / abs(vGoSub[i]);
            Min = i;
        }
    }
    //只修正最小一个分量
    if (vGoSub[Min] > 0)
    {
        vGoSub[Min] = vGoSub[Min] - inner[Min];
    }else{
        vGoSub[Min] = vGoSub[Min] + inner[Min];
    }
    //最后生成修正值
    vGo = vGoSub[0] * vSub[0] + vGoSub[1] * vSub[1] + vGoSub[2] * vSub[2];
    return true;

}


void CCubeBox::Render()
{
    //m_device->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
    //m_device->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

    //m_device->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_device->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX_BOX));
    m_device->SetIndices(m_pIB);
    m_device->SetFVF(VERTEX_BOX::FVF);
    //TextureSelect(m_pTexture);
    m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
        0,
        0,
        8*3,
        0,
        6*2);
}

void CCubeBox::ComputeBoundBox()
{
    m_boundBox = BoundBox(Point3(m_Vertices[0].x, m_Vertices[0].y, m_Vertices[0].z),
        Point3(m_Vertices[5].x, m_Vertices[5].y, m_Vertices[5].z));
}