#include "Animation3D.h"


CAnimation3D::CAnimation3D() : CObject3D()
{
    m_curObj = NULL;
}

CAnimation3D::~CAnimation3D()
{

}

HRESULT CAnimation3D::Init(LPDIRECT3DDEVICE9 device)
{
    CObject3D::Init(device);
    for (size_t i = 0; i < m_list.size(); i++)
        m_list[i]->Init(device);
    if (CurIndex >=0 && CurIndex < Count())
        m_curObj = m_list[CurIndex];
    return S_OK;
}

void CAnimation3D::Render()
{
    if (m_curObj)
        m_curObj->Render();
}

void CAnimation3D::Destroy()
{
}

void CAnimation3D::ComputeBoundBox(const D3DXMATRIX* mat)
{
    for (size_t i = 0; i < m_list.size(); i++)
        m_list[i]->ComputeBoundBox(mat);
}

LPDIRECT3DTEXTURE9 CAnimation3D::GetTexture() const
{
    if (m_curObj)
        return m_curObj->GetTexture();
    else
        return NULL;
}

size_t CAnimation3D::SamplePoints(size_t nSamples, size_t nMax, Point3* positions, Point3* normals)
{
    if (m_curObj)
        return SamplePoints(nSamples, nMax, positions, normals);
    else
        return 0;
}

float CAnimation3D::ComputeArea(const D3DXMATRIX* mat)
{
    if (m_curObj)
        return m_curObj->ComputeArea(mat);
    else
        return 0;
}

bool CAnimation3D::Collision(D3DXVECTOR3& vGo, D3DXVECTOR3& vPos)
{
    if (m_curObj)
        return m_curObj->Collision(vGo, vPos);
    else
        return false;
}

const BoundBox& CAnimation3D::GetBoundBox() const
{
    if (m_curObj)
        return m_curObj->GetBoundBox();
    else
        return m_boundBox;
}

void CAnimation3D::Update(float elapsedTime)
{
    if (Stopped) return;
    //计时
    m_counter += elapsedTime;
    //超过的时间
    while (m_counter >= DelayPerFrame)
    {
        //跳到下一帧
        if (CurIndex >= EndFrame)
        {
            if (Looped)	
                CurIndex = 0;
            else 
                break;
        }
        else 
            CurIndex ++;
        m_counter -= DelayPerFrame;
    }
    if (CurIndex >=0 && CurIndex < Count())
        m_curObj = m_list[CurIndex];
}