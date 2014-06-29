/************************************************************************/
/*  Direct3D GameEngine													*/
/*  3DÎïÌå¶ÔÏó                            ±àÐ´£º Ò¦´ºêÍ                 */
/************************************************************************/

#pragma once
#include <D3DX9.h>
#include <string>
#include "MyLib.h"
#include "BoundBox.h"

class CObject3D
{
protected:
    FLOAT m_x, m_y, m_z;

    FLOAT m_roll;           //ºá¹ö
    FLOAT m_pitch;          //À­Éý
    FLOAT m_yaw;            //Æ«º½

    size_t m_vertexSize;
    size_t m_indexSize;
    size_t m_vertexCount;
    size_t m_indexCount;

    LPDIRECT3DDEVICE9 m_device;

    D3DXMATRIXA16  m_matWorld;
    D3DXMATRIXA16  m_matExtra;
    D3DXMATRIXA16  m_matRot;

	LPDIRECT3DVERTEXBUFFER9 m_pVB; 
	LPDIRECT3DINDEXBUFFER9 m_pIB;

    BoundBox m_boundBox;

    std::string m_name;

    bool m_isVisible;

public:
    HRESULT virtual Init(LPDIRECT3DDEVICE9 device);

    void virtual Render() = 0;
    void virtual Update(float elapsedTime) {};
    void virtual Destroy();

    void virtual SetPosition(FLOAT x, FLOAT y, FLOAT z);
    void virtual SetRotation(FLOAT roll, FLOAT pitch, FLOAT yaw);
    Point3 virtual GetPosition() { return Point3(m_x, m_y, m_z); };
    Point3 virtual GetRotation() { return Point3(m_roll, m_pitch, m_yaw); };
    bool virtual GetVisible() { return m_isVisible; };
    void virtual SetVisible(bool v) { m_isVisible = v; };
    void virtual UpdateMatrix();
    void virtual ComputeBoundBox(const D3DXMATRIX* mat = NULL);
    LPDIRECT3DTEXTURE9 virtual GetTexture() const {return NULL;};
    size_t virtual SamplePoints(size_t nSamples, size_t nMax,
        Point3* positions, Point3* normals);
    
    float virtual ComputeArea(const D3DXMATRIX* mat = NULL);

	bool virtual Collision(D3DXVECTOR3& vGo, D3DXVECTOR3& vPos) {return false;};
    const BoundBox& GetBoundBox() const { return m_boundBox; }	
	LPDIRECT3DVERTEXBUFFER9 GetVB() { return m_pVB; }
	LPDIRECT3DINDEXBUFFER9  GetIB() { return m_pIB; }
	const D3DXMATRIXA16 * GetWorldMatrix() const { return &m_matWorld; }
    const D3DXMATRIXA16 * GetExtraMatrix() const { return &m_matExtra; }
    void SetExtraMatrix(const D3DXMATRIXA16& mat) { m_matExtra = mat; }

    size_t GetVertexCount() const { return m_vertexCount; }
    size_t GetFaceCount() const { return m_indexCount / 3; }
    
    const char* GetName() const { return m_name.c_str(); }
    void SetName(const char* name) { m_name = name; }

    virtual int SendMessage(const std::string& msg) { return 0; };
    CObject3D(void);
    ~CObject3D(void);
};
