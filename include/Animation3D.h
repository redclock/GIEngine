#pragma once
#include <vector>
#include "Object3D.h"


class CAnimation3D : public CObject3D
{
private:
    std::vector<CObject3D*> m_list;
    CObject3D* m_curObj;
    float m_counter;

public:
    //�Ƿ�ֹͣ����
    bool  Stopped;
    //�Ƿ�ѭ��
    bool  Looped;
    //ÿ֡����ʱ
    float DelayPerFrame;
    //��ǰ��֡��
    int   CurIndex;
    //������ʼ֡��
    int   BeginFrame;
    //��������֡��
    int   EndFrame;

public:
    int Count() const { return (int) m_list.size(); }
    CObject3D* GetFrame(int i) const { return m_list[i]; }
    void Clear() { m_list.clear(); };
    void Add(CObject3D* obj) { m_list.push_back(obj); }

public:
    CAnimation3D();
    ~CAnimation3D();

    HRESULT virtual Init(LPDIRECT3DDEVICE9 device);


    void virtual Render();
    void virtual Update(float elapsedTime);
    void virtual Destroy();

    void virtual ComputeBoundBox(const D3DXMATRIX* mat = NULL);
    LPDIRECT3DTEXTURE9 virtual GetTexture() const;
    size_t virtual SamplePoints(size_t nSamples, size_t nMax,
        Point3* positions, Point3* normals);

    float virtual ComputeArea(const D3DXMATRIX* mat = NULL);

    bool virtual Collision(D3DXVECTOR3& vGo, D3DXVECTOR3& vPos);
    const BoundBox& GetBoundBox() const;	
};