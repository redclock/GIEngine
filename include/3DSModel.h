#pragma once
#include <vector>
#include <map>
#include <string>
#include "Object3D.h"
#include "3DSLoader.h"
#include "Point3.h"
#include "Material.h"

struct VERTEX_MESH
{
    Point3 pos;
    Point3 norm;

    DWORD color;
    FLOAT  tu, tv;   // Vertex texture coordinates
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL| D3DFVF_DIFFUSE  | D3DFVF_TEX1;
};

class C3DSModel: public C3DSLoader
{
private:
    struct MaterialInfo
    {
        std::string texture;
        BYTE r, g, b;
        BYTE materialType;
    };

    class CMesh: public CObject3D
    {
    private:
        VERTEX_MESH * m_pVertices;
        DWORD m_nVertex;
        DWORD m_nFace;
        LPDIRECT3DTEXTURE9 m_pTexture;
    public:
        std::string name;
        void SetDevice(LPDIRECT3DDEVICE9 device);
        void SetTexture(LPDIRECT3DTEXTURE9 tex);
        void SetDiffuse(BYTE r, BYTE g, BYTE b, BYTE matType);
        void SetVertices(WORD num, const float* vbuf);
        void SetTexCoord(WORD num, const float* tbuf);
        void SetFaces(WORD num, const WORD* faces);
        bool Complete();
        void CalcNormals();
        void Render();
        LPDIRECT3DTEXTURE9 GetTexture() const { return m_pTexture; };
        CMesh();
        ~CMesh();
    };

    std::vector<CMesh *> m_meshes;
    std::map<std::string, MaterialInfo *> m_matdict;
    std::map<std::string, MaterialInfo *> m_matdictExtra;
    MaterialInfo * m_curMaterial;
    CMesh * m_curMesh;
protected:


    void SetVertices(WORD num, const float* vbuf);
    void SetTexCoord(WORD num, const float* tbuf);
    void SetFaces(WORD num, const WORD* faces);

    void ApplyMaterial(const char * matname);
    void SetLocalMatrix(const float* mat);

    void NewMaterial(const char * matname);
    void SetMaterialDiffuse(BYTE r, BYTE g, BYTE b);
    void SetMaterialTexture(const char * filename);

    void BeginMesh(const char * name);
    void EndMesh();

public:

    void Render();

    void AddExtraMaterial(std::string matname, std::string texture, BYTE r, BYTE g, BYTE b, BYTE matType);

    size_t GetMeshCount() const { return m_meshes.size(); }
    CObject3D* GetMesh(size_t index) const { return m_meshes[index]; }
    CObject3D* FindMesh(std::string name) const;

    C3DSModel();
    ~C3DSModel();
};